// Copyright (c) 2019-2024  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/libexcept
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
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// self
//
#include    "libexcept/report_signal.h"

#include    "libexcept/stack_trace.h"


// C++
//
#include    <cstdint>
#include    <iostream>
#include    <memory>


// C
//
#include    <signal.h>


/** \file
 * \brief Implementation of the init_report_signal() function.
 *
 * This file includes a function to setup a report handler on all the crashing
 * signals such as SEGV. This allows your software to report the stacktrace
 * even in a release version.
 *
 * \note
 * If you can link against the eventdispatcher library, you should instead
 * consider using that library signal handlers.
 */



namespace libexcept
{



namespace
{

typedef struct sigaction                sigaction_t;
typedef std::shared_ptr<sigaction_t>    sigaction_ptr_t;

sigaction_ptr_t             g_signal_actions[64] = {};

void report_signal(
          int sig
        , siginfo_t * info
        , void * context)
{
    // unused parameters
    static_cast<void>(info);
    static_cast<void>(context);

    auto const trace(collect_stack_trace());
    for(auto const & stack_line : trace)
    {
        std::cerr
            << "report_signal():"
            << sig
            << ": backtrace="
            << stack_line
            << "\n";
    }

    // Abort
    //
    abort();
}


} // no name namespace



/** \brief Setup the callbacks.
 *
 * This function sets up the callbacks of all the signals representing a
 * crash. The callback will print the stack to stderr.
 *
 * If you are a project over eventdispatcher, you have several options at
 * your disposal which are much better than this simplistic function:
 *
 * \li signal -- a connection that can catch any signal using the signalfd()
 * function (i.e. the eventdispatcher can poll on it)
 * \li signal_handler -- similar to this function, it capture signals and
 * reports them in the logger including a stack trace
 * \li signal_child -- an extension of the signal connection which provides
 * additional data about the child that died
 *
 * \warning
 * This code is not thread safe.
 */
void init_report_signal()
{
    constexpr std::int64_t sigs(
              (1 << SIGHUP)
            | (1 << SIGILL)
            | (1 << SIGTRAP)
            | (1 << SIGBUS)
            | (1 << SIGFPE)
            | (1 << SIGSEGV)
        );
    for(size_t i(0); i < std::size(g_signal_actions); ++i)
    {
        if((sigs & (1L << i)) != 0)
        {
            sigaction_t action = sigaction_t();
            action.sa_sigaction = report_signal;
            action.sa_flags = SA_SIGINFO | SA_RESETHAND;

            g_signal_actions[i] = std::make_shared<sigaction_t>();
            sigaction(i, &action, g_signal_actions[i].get());
        }
    }
}



}
// namespace libexcept
// vim: ts=4 sw=4 et
