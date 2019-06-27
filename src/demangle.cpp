/*
 * Implementation:
 *      src/demangle.cpp
 *
 * Description:
 *      The implementation that demangles C++ names.
 *
 * License:
 *      Copyright (c) 2019  Made to Order Software Corp.  All Rights Reserved
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
#include "./demangle.h"

// C++ includes
//
//#include <cstdlib>
#include <cxxabi.h>
#include <memory>


/** \file
 * \brief Implementation of the demandler functions.
 *
 * This file includes functions we can use to demangle C++ names.
 */



namespace libexcept
{



/** \brief Demangle the specified type string.
 *
 * C++ offers a `typeid(\<type>).name()` function, only that does not
 * return a readable name in many cases. This function transforms that
 * name back to the original. This is particularly useful for C++ base
 * types. For example "unsigned short" becomes "t". To verify a type,
 * it is quite practical.
 *
 * This function demangles all names, including those we get when
 * building a stack trace.
 *
 * \param[in] type_id_name  Whether to collect the stack or not.
 *
 * \return The converted name.
 *
 * \sa https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
 */
std::string demangle_cpp_name(char const * name)
{
#if 1
    int status(0);

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free
    };

    return status == 0
                ? res.get()
                : name;
#else
    // keeping the fallback in case the ABI stops working over time
    // this is "very" slow since it runs an external tool (c++filt)
    //
    std::string cppfilt("c++filt "
                      + raw_function_name);
    std::unique_ptr<FILE, decltype(&::pclose)> p(popen(cppfilt.c_str(), "r"), &::pclose);
    for(;;)
    {
        int const c(fgetc(p.get()));
        if(c == EOF)
        {
            break;
        }
        if(c != '\n')
        {
            result += c;
        }
    }
#endif
}



}
// namespace libexcept
// vim: ts=4 sw=4 et
