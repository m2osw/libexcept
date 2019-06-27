/*
 * Implementation:
 *      src/exception.cpp
 *
 * Description:
 *      The implementation that gathers the stack whenever an exception occurs.
 *      This is particularly useful to debug exceptions and possibly fix the
 *      code quickly.
 *
 * License:
 *      Copyright (c) 2011-2019  Made to Order Software Corp.  All Rights Reserved
 * 
 *      https://snapwebsites.org/
 *      contact@m2osw.com
 * 
 *      Permission is hereby granted, free of charge, to any person obtaining a
 *      copy of this software and associated documentation files (the
 *      "Software"), to deal in the Software without restriction, including
 *      without limitation the rights to use, copy, modify, merge, publish,
 *      distribute, sublicense, and/or sell copies of the Software, and to
 *      permit persons to whom the Software is furnished to do so, subject to
 *      the following conditions:
 *
 *      The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 *      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "./exception.h"

// libexcept lib
//
#include "./demangle.h"

// boost lib
//
#include <boost/algorithm/string/replace.hpp>

// C++ includes
//
#include <iostream>
#include <memory>
#include <sstream>

// C lib includes
//
#include <execinfo.h>
#include <unistd.h>


/** \file
 * \brief Implementation of the libexcept classes.
 *
 * This file includes the library implementation. Especially, it has the
 * code that generates a stack trace and converts the results to a C++
 * vector of strings.
 */


/** \mainpage
 *
 * The libexcept library offers us a way to automatically get a stack trace
 * every time an exception occurs.
 *
 * \section introduction Introduction
 *
 * The Snap! C++ environment uses a lot of exceptions, but nearly only when
 * the exception can't be avoided (i.e. more or less a fatal error in the
 * current situation.) Therefore, having a way to immediately discover where
 * the exception occurred by using the libexcept exception classes gives you
 * a way to immediately find out which function raised the exception nearly
 * 99% of the time including in the runtime environment of Snap! C++ and
 * any other project using the libexcept library.
 *
 * \section classes Classes to Derive From
 *
 * This library gives you two exception classes to derive from:
 *
 * \subsection logic_exception libexcept::logic_exception_t
 *
 * Used to raise an exception about logic; although this is often an
 * "emergency" type of error (even worse than a fatal error), we have
 * a definitions for it because we raise many logic errors.
 *
 * Example of a logic error:
 *
 * A function is expected to receive two parameters, say both are enumerations.
 * When the first enumeration is set to `FOO` then the second is expected
 * to be one of `BAR` or `BAZ`. If the second is set to `NOT_SO_GOOD` instead,
 * then the function raises a logic error because the programmer made a
 * mistake and the problem can be fixed by fixing the code (i.e. once the
 * code is fixed, you should then never see the error again.)
 *
 * \subsection runtime_exception libexcept::exception_t
 *
 * Used for most of our exceptions. This is based on the
 * `std::runtime_error` base class.
 *
 * \section object_stack_trace Collect a Stack Trace Creating an Object
 *
 * You may also use the libexcept::exception_base_t class directly in your
 * class(es) in order to collect a stack trace at the time the class is
 * instantiated. The libexcept::exception_base_t::get_stack_trace()
 * gives you the results.
 *
 * \code
 *      libexcept::exception_base_t stack_info;
 *      libexcept::stack_trace_t stack_dump(stack_info.get_stack_trace());
 *      ...here `stack_dump` is a vector of strings, one string represents one frame...
 * \endcode
 *
 * By default we use STACK_TRACE_DEPTH as the number of stings to return
 * in the libexcept::stack_trace_t vector.
 *
 * \section in_place_stack_trace Collect a Stack Trace Anywhere
 *
 * Finally, you can directly call the libexcept::collect_stack_trace()
 * function since it is a global function. It gives you a vector of
 * strings representing the stack trace.
 *
 * We also offer the libexcept::collect_stack_trace_with_line_numbers()
 * for debug only. The exceptions do not make use of that function by
 * default because it is way too slow. It is useful to convert the
 * frame IP addresses to line numbers (assuming you still have debug
 * information in your text files and the software can find the text
 * file concerned by the problem.)
 *
 * \section thread_safety Thread Safety
 *
 * The library is thread safe. All the functions are reentrant except
 * the set_collect_stack(), which is still safe to use, only the
 * results may not always be exactly as expected.
 *
 * In terms of parallelism, the collect_stack_trace_with_line_numbers()
 * runs some console processes to collect the line number and demangle
 * the function names. This means that it could be really heavy if many
 * threads use that function often.
 */



namespace libexcept
{


namespace
{


/** \brief Global flag to eventually prevent stack trace collection.
 *
 * Whenever a libexcept exception is raised, the stack gets collected.
 * This is very slow if you run a test which is to generate exceptions
 * over and over again, like 1,000,000 times in a tight loop.
 *
 * To make your tests faster we added a general flag which one can use
 * to collect or not collect the stack trace.
 *
 * At some point we may add an option to our command lines/configuration
 * files to tweak this flag on load. That way any of our daemon can
 * benefit by not having a stack trace in a production environment unless
 * requested. Rmember, though, that we use exceptions wisely so they really
 * only happens when something really bad is detected so it is fairly
 * safe to keep the collection of the stack trace turned on.
 */
bool            g_collect_stack = true;


}


/** \brief Set a general flag on whether to collect stack traces or not.
 *
 * Because collecting the stack trace can be time consuming and once in
 * a while you may need the highest possible speed including libexcept
 * exceptions, we offer a flag to avoid all stack collection processing.
 *
 * We especially use this feature when running tests because we generate
 * the exceptions on purpose and do not want to get the stack trace which
 * is rather useless in this case. We do not yet have any other situations
 * where we do not want a stack trace.
 *
 * By default \p collect_stack is already true so you do not need to change
 * it on startup.
 *
 * \warning
 * The function itself is not multithread safe. It is unlikely to cause
 * any serious problems, though. Some threads may have or may be missing
 * the stack trace, that's all. If you never call this function, all threads
 * will always include the stack trace. Calling this function before you
 * create threads will resolve all possible issues (if you do not have
 * to dynamically change the flag.)
 *
 * \param[in] collect_stack  Whether to collect the stack or not.
 */
void set_collect_stack( bool collect_stack )
{
    g_collect_stack = collect_stack;
}


/** \brief Collect the stack trace in a list of strings.
 *
 * This function collects the current stack as a trace to log later.
 *
 * By default, the stack trace shows you a number of backtrace equal
 * to STACK_TRACE_DEPTH (which is 20 at time of writing). You may
 * specify another number to get more or less lines. Note that a
 * really large number will generally show you the entire stack since
 * a number larger than the number of function pointers on the stack
 * will return the entire stack.
 *
 * If you pass 0 as \p stack_trace_depth then the function returns an
 * empty vector of strings.
 *
 * Passing `std::numeric_limits<int>::max()` as the \p stack_trace_depth
 * parameter forces the function to return the entire stack trace available.
 *
 * \note
 * This function is global so we can use it anywhere we'd like to get
 * a stack trace and not just in exceptions. Very practical in C++
 * to get a stack trace directly in a vector of strings.
 *
 * \note
 * This function is not affected by the g_collect_stack flag. So you can
 * always collect a stack trace. Only exceptions do not do so automatically
 * if you set the g_collect_stack flag to false.
 *
 * \important
 * Use the collect_stack_with_line_numbers() to get demangled function
 * names and line numbers. Note that this other function is considered
 * \em very slow so do not use it in a standard exception. Consider
 * using that other function only when debugging.
 *
 * \param[in] stack_trace_depth  The number of lines to capture in our
 *                               stack trace.
 * \return The vector of strings with the stack trace.
 *
 * \sa collect_stack_trace_with_line_numbers()
 * \sa set_collect_stack()
 */
stack_trace_t collect_stack_trace( int stack_trace_depth )
{
    stack_trace_t stack_trace;

    if(stack_trace_depth > 0)
    {
        std::vector<void *> array;
        array.resize( stack_trace_depth );
        int const size(backtrace( &array[0], stack_trace_depth ));

        // save a copy of the system array in our class
        //
        std::unique_ptr<char *, decltype(&::free)> stack_string_list(backtrace_symbols( &array[0], size ), &::free);
        for( int idx(0); idx < size; ++idx )
        {
            char const * stack_string( stack_string_list.get()[idx] );
            stack_trace.push_back(stack_string);
        }
    }

    return stack_trace;
}












/** \brief Collect the stack trace in a list of strings.
 *
 * This function collects the current stack as a trace including
 * the line numbers and demangled function names as available.
 *
 * The function also works like the collect_stack_trace() function.
 *
 * \note
 * The function makes use of the `addr2line` and `c++filt` command
 * line tools to convert the information. It is likely that if it
 * fails it means your system does not have those two tools installed.
 * Also, the addr2line requires the debug information in the libraries
 * and executables. Without that information you will still get invalid
 * answers in your stacktrace.
 *
 * See also the libbacktrace library:
 * https://gcc.gnu.org/viewcvs/gcc/trunk/libbacktrace/
 *
 * \note
 * This function is not affected by the g_collect_stack flag. So you can
 * always collect a stack trace. Only exceptions do not do so automatically
 * if you set the g_collect_stack flag to false.
 *
 * \todo
 * I found a piece of code snippet on Catch2 which is used to demangle a C++
 * name. It is one simple ABI call! We would still need the translations of
 * the IP address to a function name and line number, though.
 *
 * \code
 * #include "catch.hpp"
 *
 * #include <cxxabi.h>
 * #include <typeinfo>
 *
 * CATCH_TRANSLATE_EXCEPTION(std::exception& e) {
 *   std::string s;
 *   int status;
 *
 *   const char* name = typeid(e).name();
 *   char* realname = abi::__cxa_demangle(name, 0, 0, &status);
 *   if(realname) {
 *     s.append(realname);
 *   } else {
 *     s.append(name);
 *   }
 *   s.append(": ");
 *   s.append(e.what());
 *   free(realname);
 *   return s;
 * }
 * \endcode
 *
 * Source: https://github.com/catchorg/Catch2/issues/539
 *
 * \param[in] stack_trace_depth  The number of lines to capture in our
 *                               stack trace.
 * \return The vector of strings with the stack trace.
 *
 * \sa collect_stack_trace()
 * \sa set_collect_stack()
 */
stack_trace_t collect_stack_trace_with_line_numbers( int stack_trace_depth )
{
    stack_trace_t stack_trace;

    if(stack_trace_depth > 0)
    {
        std::vector<void *> array;
        array.resize( stack_trace_depth );
        int const size(backtrace( &array[0], stack_trace_depth ));

        // save a copy of the system array in our class
        //
        std::unique_ptr<char *, decltype(&::free)> stack_string_list(backtrace_symbols( &array[0], size ), &::free);
        for( int idx(0); idx < size; ++idx )
        {
            char const * raw_stack_string( stack_string_list.get()[idx] );

            // the raw stack string is expected to be composed of:
            //   <filename>(<function-name>+<offset>) [<addr>]
            //
            // we extract all those elements and use addr2line and c++filt
            // to convert that data to a usable function name and line number
            //
            std::string filename;
            std::string raw_function_name;
            std::string addr;
            std::string result;

            // go to end of filename
            //
            char const * s(raw_stack_string);
            char const * start(s);
            while(*s != '('
               && *s != '\0')
            {
                ++s;
            }
            if(*s == '(')
            {
                filename = std::string(start, s - start);
                ++s;

                start = s;
                while(*s != '+'
                   && *s != ')'
                   && *s != '\0')
                {
                    ++s;
                }

                if(*s == '+')
                {
                    raw_function_name = std::string(start, s - start);
                    ++s;

                    // skip the offset
                    //
                    while(*s != ')'
                       && *s != '\0')
                    {
                        ++s;
                    }
                }
                //else if(*s == ')') {} -- no function name

                if(*s == ')'
                && s[1] == ' '
                && s[2] == '[')
                {
                    s += 3;

                    start = s;
                    while(*s != ']'
                       && *s != '\0')
                    {
                        ++s;
                    }

                    if(*s == ']')
                    {
                        addr = std::string(start, s - start);

                        result.clear();

                        // here we have our info, use it to get sane data
                        //
                        {
                            boost::replace_all(filename, "'", "\\'");
                            std::string addr2line("eu-addr2line --pid="
                                            + std::to_string(getpid())
                                            //+ " -e '"
                                            //+ filename
                                            //+ "' "
                                            + " "
                                            + addr);
                            std::unique_ptr<FILE, decltype(&::pclose)> p(popen(addr2line.c_str(), "r"), &::pclose);
                            std::string line;
                            for(;;)
                            {
                                int const c(fgetc(p.get()));
                                if(c == EOF)
                                {
                                    break;
                                }
                                if(c != '\n')
                                {
                                    line += c;
                                }
                            }
                            if(line == "??:0")
                            {
                                // this means addr2line failed to find the
                                // debug info in your executable/.so
                                // we fallback to the default which may help
                                // if you have a version with debug info
                                //
                                result += filename
                                        + "["
                                        + addr
                                        + "]";
                            }
                            else
                            {
                                result = line;
                            }
                        }

                        if(!raw_function_name.empty())
                        {
                            result += " in ";
                            result += demangle_cpp_name(raw_function_name.c_str());
                        }
                        else
                        {
                            result += " <no function name>";
                        }
                    }
                }
            }

            if(result.empty())
            {
                // use the raw line as a fallback if we could not parse it
                // correctly or a conversion somehow fails...
                //
                stack_trace.push_back(raw_stack_string);
            }
            else
            {
                stack_trace.push_back(result);
            }
        }
    }

    return stack_trace;
}












/** \var int libexcept::exception_base_t::STACK_TRACE_DEPTH
 * \brief Default depth of stack traces collected.
 *
 * This parameter defines the default number of lines returned by the
 * collect_stack_trace() function.
 *
 * All the functions that call the collect_stack_trace() have a
 * `stack_trace_depth` parameter you can use to change this value.
 *
 * Note that a value of 0 is valid as the stack trace depth. This
 * just means not even one line is going to be taken from the
 * stack.
 *
 * \important
 * It is to be noted that since a few functions from the libexcept are
 * going to be included in your stack trace, using a very small depth
 * such as 1 or 2 is not going to be helpful at all. You would only
 * get data about the libexcept functions instead of the actual function
 * that generated the error.
 */


/** \var stack_trace_t libexcept::exception_base_t::f_stack_trace
 * \brief The variable where the exception stack trace gets saved.
 *
 * This parameter holds the vector of strings representing the stack
 * trace at the time an exception was raised and an instance of
 * the exception_base_t class was created.
 */


/** \var typedef std::vector<std::string> libexcept::exception_base_t::stack_trace_t
 * \brief The stack trace results.
 *
 * This typedef defines the type of the variables used to pass the stack
 * trace between functions. It is a simple vector a C++ strings.
 *
 * The first string (`trace[0]`) represents the current function. Note that
 * the collected functions will include all the functions, including the
 * exception_base_t::collect_stack_trace() and various calling functions
 * from the libexcept library. In most cases this means 2 or 3 lines at
 * the start of the stack trace vector are going to be about libexcept
 * functions and not the function where the exception was raised.
 */


/** \brief Initialize this Snap! exception.
 *
 * Initialize the base exception class by generating the output of
 * a stack trace to a list of strings.
 *
 * \warning
 * At this time every single exception derived from exception_t generates
 * a stack trace. Note that in most cases, our philosophy is to generate
 * exceptions only in very exceptional cases and not on every single error
 * so the event should be rare in a normal run of our daemons.
 *
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 *
 * \sa collect_stack_trace()
 */
exception_base_t::exception_base_t( int const stack_trace_depth )
{
    if(g_collect_stack)
    {
        f_stack_trace = collect_stack_trace( stack_trace_depth );
    }
}


/** \fn exception_base_t::~exception_base_t()
 * \brief Destructor of the exception base class.
 *
 * This destructor is defined to ease derivation when some of the classes
 * have virtual functions.
 */



/** \fn exception_base_t::get_stack_trace()
 * \brief Retrieve the stack trace.
 *
 * This function retreives a reference to the vector of strings representing
 * the stack trace at the time the exception was raised.
 */

/** \brief Initialize an exception from a C++ string.
 *
 * This function initializes an exception settings its 'what' string to
 * the specified \p what parameter.
 *
 * \note
 * Logic exceptions are used for things that just should not ever happen.
 * More or less, a verification of your class contract that fails.
 *
 * \param[in] what  The string used to initialize the exception what parameter.
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 */
logic_exception_t::logic_exception_t( std::string const & what, int const stack_trace_depth )
    : std::logic_error(what.c_str())
    , exception_base_t(stack_trace_depth)
{
}


/** \fn logic_exception_t::~logic_exception_t()
 * \brief Destructor of the logic exception class.
 *
 * This destructor is defined to ease derivation when some of the classes
 * have virtual functions.
 */


/** \brief Initialize an exception from a C string.
 *
 * This function initializes an exception settings its 'what' string to
 * the specified \p what parameter.
 *
 * \note
 * Logic exceptions are used for things that just should not ever happen.
 * More or less, a verification of your class contract that fails.
 *
 * \param[in] what  The string used to initialize the exception what parameter.
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 */
logic_exception_t::logic_exception_t( char const * what, int const stack_trace_depth )
    : std::logic_error(what)
    , exception_base_t(stack_trace_depth)
{
}


/** \brief Retrieve the `what` parameter as passed to the constructor.
 *
 * This function returns the `what` description of the exception when the
 * exception was initialized.
 *
 * \note
 * We have an overload because of the dual derivation.
 *
 * \return A pointer to the what string. Must be used before the exception
 *         gets destructed.
 */
char const * logic_exception_t::what() const throw()
{
    return std::logic_error::what();
}












/** \brief Initialize an exception from a C++ string.
 *
 * This function initializes an exception settings its 'what' string to
 * the specified \p what parameter.
 *
 * \param[in] what  The string used to initialize the exception what parameter.
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 */
exception_t::exception_t( std::string const & what, int const stack_trace_depth )
    : std::runtime_error(what.c_str())
    , exception_base_t(stack_trace_depth)
{
}


/** \fn exception_t::~exception_t()
 * \brief Destructor of the exception class.
 *
 * This destructor is defined to ease derivation when some of the classes
 * have virtual functions.
 */


/** \brief Initialize an exception from a C string.
 *
 * This function initializes an exception settings its 'what' string to
 * the specified \p what parameter.
 *
 * \param[in] what  The string used to initialize the exception what parameter.
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 */
exception_t::exception_t( char const * what, int const stack_trace_depth )
    : std::runtime_error(what)
    , exception_base_t(stack_trace_depth)
{
}


/** \brief Retrieve the `what` parameter as passed to the constructor.
 *
 * This function returns the `what` description of the exception when the
 * exception was initialized.
 *
 * \note
 * We have an overload because of the dual derivation.
 *
 * \return A pointer to the what string. Must be used before the exception
 *         gets destructed.
 */
char const * exception_t::what() const throw()
{
    return std::runtime_error::what();
}


}
// namespace libexcept
// vim: ts=4 sw=4 et
