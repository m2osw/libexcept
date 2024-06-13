// Copyright (c) 2019-2024  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/
// contact@m2osw.com
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

/** \file
 * \brief Handle sigmask in a scoped manner (RAII).
 *
 * This class is used to manage the signal mask in a scoped manner.
 */

// C++
//
#include    <list>
#include    <memory>


// C
//
#include    <signal.h>



namespace libexcept
{



typedef std::list<int>                  sig_list_t;


class scoped_signal_mask
{
public:
    typedef std::shared_ptr<scoped_signal_mask> pointer_t;

                    scoped_signal_mask(sig_list_t block_signals = sig_list_t());
                    ~scoped_signal_mask();

private:
    bool            f_set = false;
    sigset_t        f_original_mask = sigset_t();
};


bool has_sanitizer();



}
// namespace libexcept
// vim: ts=4 sw=4 et
