// Copyright (c) 2019-2025  Made to Order Software Corp.  All Rights Reserved
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
#include    "libexcept/scoped_signal_mask.h"

#include    "libexcept/exception.h"


// C++
//
#include    <iostream>


// C
//
#include    <dlfcn.h>
#include    <signal.h>


/** \file
 * \brief Handle sigmask in a scoped manner (RAII).
 *
 * This class is used to manage the signal mask in a scoped manner.
 */



namespace libexcept
{



namespace
{



constexpr int const g_incompatible_signals[] = {
    SIGHUP,
    SIGINT,
    SIGILL,
    SIGABRT,
    SIGFPE,
    SIGKILL,    // this one is not necessary rather than incompatible
    SIGSEGV,
    SIGTERM,
    SIGCONT,
    SIGSTOP,
    SIGTSTP,
    SIGPROF,
    SIGIO,
};


bool g_defined = false;
bool g_has_sanitizer = false;



} // no name namespace



/** \brief Set signal mask.
 *
 * This constructor blocks the specified \p block_signals and the
 * destructor restores the flags as expected once the object is
 * out of scope.
 *
 * If the \p block_signals list is empty, then all the signals get
 * blocked. This is particularly useful to start a new thread.
 *
 * \param[in] block_signals  The list of signals to block or empty
 * to block them all.
 */
scoped_signal_mask::scoped_signal_mask(sig_list_t block_signals)
{
    sigset_t set;
    sigemptyset(&set);

    if(block_signals.empty())
    {
        // sigfillset() does not set the few signals that should never be
        // blocked (would be ignored by the command below)
        //
        sigfillset(&set);
    }
    else
    {
        for(auto const & sig : block_signals)
        {
            if(sigaddset(&set, sig) != 0)
            {
                throw fixme("sigaddset() failed to set signal " + std::to_string(sig));
            }
        }
    }

    if(has_sanitizer())
    {
        for(auto const & sig : g_incompatible_signals)
        {
            if(sigdelset(&set, sig) != 0)
            {
                throw fixme("sigdelset() failed to delete signal " + std::to_string(sig));
            }
        }
    }

    if(sigprocmask(SIG_BLOCK, &set, &f_original_mask) != 0)
    {
        throw fixme("sigprocmask() failed to block signals.");
    }

    f_set = true;
}


/** \brief Reset signal mask the way it was before the constructor was called.
 *
 * This function restores the signals as they were before the
 * scoped_signal_mask was created.
 */
scoped_signal_mask::~scoped_signal_mask()
{
    if(f_set)
    {
        if(sigprocmask(SIG_SETMASK, &f_original_mask, nullptr) != 0)
        {
            std::cerr << "fatal error: sigprocmask() failed to block signals.\n";
            std::terminate();
        }
    }
}


/** \brief Check whether this instance is running with the sanitizer.
 *
 * This funciton returns true if the software was compiled with the
 * sanitizer. This test happens at runtime. The test results are saved
 * in global variables. To make it thread safe, it should be called
 * once before starting any thread.
 *
 * Note that by default the sanitizer is enabled, but it is possible
 * to disable it using the __lsan_disable() function. There is no
 * function I've seen that will tell you whether the sanitizer is
 * currently enabled or disabled.
 *
 * \return true if the running process has the sanitizer linked in.
 */
inline bool has_sanitizer()
{
    if(!g_defined)
    {
        g_defined = true;
        g_has_sanitizer = dlsym(RTLD_DEFAULT, "__lsan_enable") != nullptr;
    }

    return g_has_sanitizer;
}



}
// namespace libexcept
// vim: ts=4 sw=4 et
