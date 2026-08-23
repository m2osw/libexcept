// Copyright (c) 2011-2025  Made to Order Software Corp.  All Rights Reserved
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
#include    <backtrace.h>
#include    <execinfo.h>
#include    <link.h>
#include    <unistd.h>


/** \file
 * \brief Implementation of the stack trace.
 *
 * This file includes the functions that are used to gather the stack trace
 * from those very functions.
 *
 * By default, the stack trace is turned on in our exception library.
 * When turned on, we gather the information from the stack using
 * the functions defined in this file. This can be very helpful when
 * the code is able to gather the filenames and line numbers as it will
 * tell you exactly where each function failed.
 *
 * If you are running in release mode, it is likely that no line numbers
 * will be generated since they will be removed from your executables.
 */



namespace libexcept
{
namespace
{



/** \brief The extended stack flag.
 *
 * When creating a stack trace, we skip various frames by default:
 * the libexcept frames, because they are rather useless (assuming
 * this library functions as expected) and the functions called before
 * the main() function.
 *
 * You may change the behavior by calling the set_extended_stack() function.
 *
 * \note
 * The stack depth also determines how many frames of stack you will get
 * in your output. That is limited to STACK_TRACE_DEPTH (20) by default.
 * Skipped frames generally do not count against that depth.
 */
bool        g_extended_stack        = false;


/** \brief Capture errors while running backtrace_pcinfo().
 *
 * When trying to collect information about a function, we call the
 * backtrace_pcinfo() function with this function as a callback function
 * for errors. It generates an error message in std::cerr with the
 * library's message and err number.
 *
 * \param[in] data  Our data pointer.
 * \param[in] msg  The library's message.
 * \param[in] err  The error number.
 */
void error_callback(void * data, char const * msg, int err)
{
    static_cast<void>(data);

    std::cerr
        << "libbacktrace error: "
        << msg
        << " ("
        << err
        << ")\n";
}


/** \brief Retrieve a pointer to the backtrace state.
 *
 * To use the backtrace_pcinfo() function, we need to have a backtrace
 * state. Function safely allocate that state the first time it gets
 * called. We automatically create a thread safe version of the state.
 *
 * \note
 * This state leaks.
 *
 * \return A pointer to the backtrace_state object.
 */
backtrace_state * get_backtrace_state()
{
    static backtrace_state * g_backtrace_state = backtrace_create_state(
              nullptr
            , 1
            , error_callback
            , nullptr);

    return g_backtrace_state;
}


/** \brief Structure used to collect symbol information.
 *
 * The backtrace library calls a callback which collects the data that
 * it has available: PC, filename, function, line number.
 *
 * Since it may need more than one entry (i.e. for example, an inlined
 * function will return itself and the caller's data,) we also offer
 * a list of symbol_info.
 */
struct symbol_info
{
    typedef std::list<symbol_info>  list_t;

    uintptr_t       f_pc        = 0;
    std::string     f_filename  = std::string();
    std::string     f_function  = std::string();
    int             f_line      = 0;
};


/** \brief Call back with backtrace data.
 *
 * This function gets called by the backtrace_pcinfo() function each time
 * it finds a match.
 *
 * The function is used to collect the filename, line number and function
 * name of each hit.
 *
 * \note
 * There can be multiple hits because you used macros, templates, functions
 * get inlined, and a few other cases.
 *
 * \param[in] data  A pointer to our structure.
 * \param[in] pc  The concerned pointer counter (memory address).
 * \param[in] filename  The name of the file with the source code.
 * \param[in] line  The line number where the call happened.
 * \param[in] function  The name of the function being called.
 *
 * \return Always 0 so we can capture all the possible names.
 */
int backtrace_callback(
      void * data
    , uintptr_t pc
    , char const * filename
    , int line
    , char const * function)
{
    symbol_info::list_t * info_list(static_cast<symbol_info::list_t *>(data));

    symbol_info info{
        .f_pc = pc,
        .f_filename = filename == nullptr ? std::string() : filename,
        .f_function = function == nullptr ? std::string() : demangle_cpp_name(function),
        .f_line = line
    };

    info_list->push_back(info);

    return 0;
}



}
// no name namespace



/** \brief Set the extended stack flag status.
 *
 * The collect_stack_trace_with_line_numbers() function has the ability
 * to skip some frames, especially the libexcept specific stack entries
 * and anything before main() gets called. The library is actually setup
 * to do just that on initialization.
 *
 * This function lets you change that before. Set the \p extended_stack
 * parameter to true in order to get the libexcept functions and the
 * functions before main().
 *
 * \param[in] extended_stack  Set to true to get the extended stack trace.
 */
void set_extended_stack(bool extended_stack)
{
    g_extended_stack = extended_stack;
}


/** \brief Get the extended stack flag status.
 *
 * The collect_stack_trace_with_line_numbers() function has the ability
 * to skip some frames, especially the libexcept specific stack entries
 * and anything before main() gets called.
 *
 * This function tells you whether those extra functions will show
 * up (true) or not (false).
 *
 * \return Whether the extended stack trace is returned or not.
 */
bool get_extended_stack()
{
    return g_extended_stack;
}


/** \brief Collect the raw stack trace in a list of strings.
 *
 * This function collects the current stack as a trace to print
 * out or log later.
 *
 * By default, the stack trace shows you a number of lines equal to
 * STACK_TRACE_DEPTH (which by default is 20 at time of writing). You
 * may specify another number to get more or less lines. Note that a
 * really large number will generally show you the entire stack since
 * a number larger than the number of function pointers on the stack
 * returns the entire stack.
 *
 * If you pass 0 as \p stack_trace_depth then the function returns an
 * empty list of strings.
 *
 * The \p stack_trace_depth parameter is silently clamped to 1,000, which
 * in most cases will be more than enough to get the entire stack trace
 * available.
 *
 * \note
 * This function is global so we can use it anywhere we'd like to get
 * a stack trace and not just in exceptions. Very practical * to get a
 * stack trace from anywhere.
 *
 * \attention
 * Use the collect_stack_with_line_numbers() to get demangled function
 * names and line numbers. Note that this other function is considered
 * to be slower, so do not use it in a standard exception. Consider
 * using that other function only when debugging.
 *
 * \param[in] stack_trace_depth  The number of lines to capture in our
 *                               stack trace.
 *
 * \return The vector of strings with the stack trace.
 *
 * \sa collect_stack_trace_with_line_numbers()
 * \sa set_collect_stack()
 */
stack_trace_t collect_stack_trace(int stack_trace_depth)
{
    stack_trace_t stack_trace;

    if(stack_trace_depth > 0)
    {
        std::vector<void *> array;
        array.resize(std::max(stack_trace_depth, 1'000));
        int const size(backtrace(&array[0], stack_trace_depth));

        // save a copy of the system array in our class
        //
        std::unique_ptr<char *, decltype(&::free)> stack_string_list(backtrace_symbols(&array[0], size), &::free);
        if(stack_string_list != nullptr)
        {
            for(int idx(0); idx < size; ++idx)
            {
                char const * stack_string(stack_string_list.get()[idx]);
                stack_trace.push_back(stack_string);
            }
        }
    }

    return stack_trace;
}


/** \brief Collect the stack trace in a list of strings.
 *
 * This function collects the current stack as a trace including
 * the line numbers and demangled function names as available.
 *
 * The function works like the collect_stack_trace() function.
 *
 * Note that by default exception do not use this function since it is
 * slower. To force the system to do so, make sure to call the following:
 *
 * \code
 *     // request complete stack info on exception
 *     //
 *     libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
 * \endcode
 *
 * \note
 * The function now uses the libbacktrace.a library. This means all the
 * backtrace work happens within our process and except for the very first
 * call, it is extremely fast. Note that for best results, you need to
 * compiled most everything with debug information.
 *
 * See also the libbacktrace library:
 * https://gcc.gnu.org/viewcvs/gcc/trunk/libbacktrace/
 *
 * \param[in] stack_trace_depth  The number of lines to capture in our
 *                               stack trace.
 * \return The vector of strings with the stack trace.
 *
 * \sa collect_stack_trace()
 * \sa set_collect_stack()
 * \sa set_extended_stack()
 */
stack_trace_t collect_stack_trace_with_line_numbers(int stack_trace_depth)
{
    stack_trace_t stack_trace;

    if(stack_trace_depth > 0)
    {
        std::vector<void *> array;
        // the libexcept adds 3 frames, hence that +3
        array.resize(std::max(stack_trace_depth + 3, 1'000));
        int const size(backtrace(&array[0], stack_trace_depth));
        backtrace_state * state(get_backtrace_state());
        for(int idx(0); idx < size; ++idx)
        {
            if(state != nullptr)
            {
                symbol_info::list_t info_list;
                backtrace_pcinfo(
                      state
                    , reinterpret_cast<uintptr_t>(array[idx])
                    , backtrace_callback
                    , error_callback
                    , &info_list);
                bool found(false);
                for(auto & info : info_list)
                {
                    if(!info.f_filename.empty())
                    {
                        found = true;
                        if(!g_extended_stack
                        && info.f_function.starts_with("libexcept::"))
                        {
                            // don't include the libexcept::... frames; it's
                            // always the same and not really useful
                            //
                            continue;
                        }
                        if(info.f_function == "main")
                        {
                            if(!g_extended_stack)
                            {
                                // done after that one
                                //
                                idx = size;
                            }

                            // this is likely correct, although it could include
                            // the environment and that won't show here
                            //
                            info.f_function = "main(int, char *[])";
                        }
                        std::string result(info.f_filename);
                        result += ':';
                        result += std::to_string(info.f_line);
                        result += " in ";
                        result += info.f_function;
                        stack_trace.push_back(result);
                    }
                }
                if(found)
                {
                    continue;
                }
            }

            // use ugly fallback; in most cases those are for system
            // libraries that do not have symbols available
            //
            std::unique_ptr<char *, decltype(&::free)> stack_string_list(backtrace_symbols(&array[idx], 1), &::free);
            if(stack_string_list.get() != nullptr)
            {
                stack_trace.push_back(stack_string_list.get()[0]);
            }
        }
    }

    return stack_trace;
}




}
// namespace libexcept
// vim: ts=4 sw=4 et
