// Copyright (c) 2011-2022  Made to Order Software Corp.  All Rights Reserved
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
#include    "./exception.h"

#include    "./demangle.h"


// C++ includes
//
#include    <iostream>
#include    <memory>
#include    <vector>


// C lib includes
//
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



/** \brief Collect the raw stack trace in a list of strings.
 *
 * This function collects the current stack as a trace to log later.
 *
 * By default, the stack trace shows you a number of lines equal
 * to STACK_TRACE_DEPTH (which is 20 at time of writing). You may
 * specify another number to get more or less lines. Note that a
 * really large number will generally show you the entire stack since
 * a number larger than the number of function pointers on the stack
 * will return the entire stack.
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
 * a stack trace and not just in exceptions. Very practical in C++
 * to get a stack trace directly in a list of strings.
 *
 * \attention
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
        for(int idx(0); idx < size; ++idx)
        {
            char const * stack_string(stack_string_list.get()[idx]);
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
 * and executables. Without that information, you will still get invalid
 * answers in your stacktrace.
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
 */
stack_trace_t collect_stack_trace_with_line_numbers(int stack_trace_depth)
{
    stack_trace_t stack_trace;

    if(stack_trace_depth > 0)
    {
        std::vector<void *> array;
        array.resize(stack_trace_depth);
        int const size(backtrace(&array[0], stack_trace_depth));

        // save a copy of the system array in our class
        //
        std::unique_ptr<char *, decltype(&::free)> stack_string_list(backtrace_symbols(&array[0], size), &::free);
        for(int idx(0); idx < size; ++idx)
        {
            char const * raw_stack_string(stack_string_list.get()[idx]);

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
                && s[2] == '['
                && s[3] == '0'
                && s[4] == 'x')
                {
                    s += 5;

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
                        // TODO: look into whether we could rewrite the
                        // eu-addr2line tool in C++ to avoid having to
                        // run it (because that would be faster)
                        //
                        // I first used the "... -e <filename> ..." option
                        // but with the --pid=..., it ought to be faster.
                        //
                        {
                            std::string addr2line("eu-addr2line --pid="
                                            + std::to_string(getpid())
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
                stack_trace.push_back(std::string(raw_stack_string));   // LCOV_EXCL_LINE
            }
            else
            {
                stack_trace.push_back(result);
            }
        }
    }

    return stack_trace;
}




}
// namespace libexcept
// vim: ts=4 sw=4 et
