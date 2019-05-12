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

// C++ includes
//
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
 * 99% of the time.
 *
 * \section classes Classes to Derive From
 *
 * This library gives you two exception classes to derive from:
 *
 * \li libexcept::logic_exception_t
 *
 * Used to raise an exception about logic; although this is often an
 * "emergency" type of error (even worse than a fatal error), we have
 * a definitions for it because we raise many logic errors.
 *
 * \li libexcept::exception_t
 *
 * Used for most of our exceptions. This is based on the
 * `std::runtime_error` base class.
 *
 * \section collect_stack_trace How to Collect a Stack Trace Anywhere
 *
 * You may also use the libexcept::exception_base_t class directly in your
 * class(es) in order to collect a stack trace. The instantiation is enough to
 * get the stack trace and the libexcept::exception_base_t::get_stack_trace()
 * gives you the results.
 *
 * \code
 *      exception_base_t stack_info;
 *      exception_base_t::stack_trace_t stack_dump(stack_info.get_stack_trace());
 *      ...here `stack_dump` is a vector of strings, one string represents one frame...
 * \endcode
 *
 * By default we use STACK_TRACE_DEPTH as the number of stings to return
 * in the exception_base_t::stack_trace_t vector.
 *
 * Finally, you can directly call the
 * libexcept::exception_base_t::collect_stack_trace() function since it is
 * a static function. It gives you a vector of strings representing the
 * stack trace.
 *
 * \section thread_safety Thread Safety
 */



namespace libexcept
{


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
    f_stack_trace = collect_stack_trace( stack_trace_depth );
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

/** \brief Collect the stack trace in a list of strings.
 *
 * This static method collects the current stack as a trace to log later.
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
 * This method is static so we can use it anywhere we'd like to get
 * a stack trace and not just in exceptions. Very practical in C++
 * to get a stack trace directly in a vector of strings.
 *
 * \todo
 * Look into demangling C++ function names. At this time only the mangled
 * version is saved in the stack trace. One concern, though, is that
 * in many cases we ignore most stack traces and therefore it would be
 * a waste of time to demangle all the time. However, having a function
 * to do the job would be great for when you want to have the functionality
 * in a specific debug session. For additional information about demangling
 * see the `c++filt` command line tool.
 *
 * \param[in] stack_trace_depth  The number of lines to capture in our
 *                               stack trace.
 * \return The vector of strings with the stack trace.
 */
exception_base_t::stack_trace_t exception_base_t::collect_stack_trace( int stack_trace_depth )
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
