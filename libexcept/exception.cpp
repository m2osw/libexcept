// Copyright (c) 2011-2024  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/
// contact@m2osw.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// self
//
#include    "libexcept/exception.h"

#include    "libexcept/demangle.h"


// C++
//
#include    <iostream>
#include    <memory>
#include    <vector>


// C
//
#include    <execinfo.h>
#include    <link.h>
#include    <unistd.h>



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
 * \subsection out_of_range_exception libexcept::out_of_range_t
 *
 * This is an extension of the std::logic_error which is expected to be used
 * whenever an out of range error occurs. This is mainly for when an index
 * is out of range when attempting to retrieve an item from an array or
 * similar concept.
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
 *      ...here `stack_dump` is a list of strings, one string per frame...
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
 * files to tweak this flag on load. That way any of our daemons can
 * benefit by not having a stack trace in a production environment unless
 * requested. Remember, though, that we use exceptions wisely so they really
 * only happens when something really bad is detected so it is fairly
 * safe to keep the collection of the stack trace turned on.
 */
collect_stack_t     g_collect_stack = collect_stack_t::COLLECT_STACK_YES;




} // no name namespace













/** \brief Tells you whether the general flag is true or false.
 *
 * This function gives you the current status of the collect stack flag.
 * If true, when exceptions will collect the stack at the time they
 * are emitted. This is very practical in debug since it gives you
 * additional information of where and possibly why an exception
 * occurred.
 */
collect_stack_t get_collect_stack()
{
    return g_collect_stack;
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
void set_collect_stack(collect_stack_t collect_stack)
{
    g_collect_stack = collect_stack;
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
 * \attention
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
exception_base_t::exception_base_t(int const stack_trace_depth)
{
    switch(get_collect_stack())
    {
    case collect_stack_t::COLLECT_STACK_NO:
        break;

    case collect_stack_t::COLLECT_STACK_YES:
        f_stack_trace = collect_stack_trace(stack_trace_depth);
        break;

    case collect_stack_t::COLLECT_STACK_COMPLETE:
        f_stack_trace = collect_stack_trace_with_line_numbers(stack_trace_depth);
        break;

    }
}


/** \fn exception_base_t::~exception_base_t()
 * \brief Destructor of the exception base class.
 *
 * This destructor is defined to ease derivation when some of the classes
 * have virtual functions.
 */


/** \brief Retrieve the set of exception parameters.
 *
 * This function returns a reference to all the parameters found in this
 * exception. In most cases, exceptions do not have parameters, however, we
 * intend to change that as we continue work on our libraries.
 *
 * \return The reference to this exception parameters.
 */
parameter_t const & exception_base_t::get_parameters() const
{
    return f_parameters;
}


/** \brief Retrieve one of the exception parameters.
 *
 * Exceptions can be assigned parameters with the set_parameter() function.
 * For example, you could include a filename as a parameter. This is useful
 * when sending logs to a database. It can simplify your searches to know
 * exact parameters instead of trying to parse strings.
 *
 * \param[in] name  The name of the parameter to search for.
 *
 * \return The value of the named parameter.
 */
std::string exception_base_t::get_parameter(std::string const & name) const
{
    auto const it(f_parameters.find(name));
    if(it == f_parameters.end())
    {
        return std::string();
    }

    return it->second;
}


/** \brief Set a parameter in this exception.
 *
 * You may add parameters to your exceptions simply by calling this function.
 *
 * Parameters are given a name. At the moment the name is not restricted,
 * however, if you want to make sure that it works in most places (i.e. in
 * the snaplogger), then you probably want to limit the name to this regex:
 *
 * \code
 *     [A-Za-z_][A-Za-z_0-9]*
 * \endcode
 *
 * Parameter values are strings.
 *
 * This is an exception, so we do not raise an exception if the name of a
 * parameter is considered invalid. At the moment, an empty string is
 * considered invalid.
 *
 * \param[in] name  The name of the parameter. It cannot be empty.
 * \param[in] value  The value of this parameter.
 */
void exception_base_t::set_parameter(std::string const & name, std::string const & value)
{
    if(name.empty())
    {
        return;
    }

    f_parameters[name] = value;
}


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
logic_exception_t::logic_exception_t(
          std::string const & what
        , int const stack_trace_depth)
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
logic_exception_t::logic_exception_t(
          char const * what
        , int const stack_trace_depth)
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
 * \note
 * Out of Range exceptions are an extension of the Logic Exception used
 * whenever a container is being accessed with an index which is too large.
 * It may also be used whenever a number doesn't fit its destination variable
 * (i.e. trying to return 300 in an `int8_t`).
 *
 * \param[in] what  The string used to initialize the exception what parameter.
 * \param[in] stack_trace_depth  The number of lines to grab in our
 *                               stack trace.
 */
out_of_range_t::out_of_range_t(
          std::string const & what
        , int const stack_trace_depth)
    : std::out_of_range(what.c_str())
    , exception_base_t(stack_trace_depth)
{
}


/** \fn out_of_range_t::~out_of_range_t()
 * \brief Destructor of the out_of_range exception class.
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
out_of_range_t::out_of_range_t(
          char const * what
        , int const stack_trace_depth)
    : std::out_of_range(what)
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
char const * out_of_range_t::what() const throw()
{
    return std::out_of_range::what();
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
exception_t::exception_t(
          std::string const & what
        , int const stack_trace_depth)
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
exception_t::exception_t(
          char const * what
        , int const stack_trace_depth)
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
