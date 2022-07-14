// Copyright (c) 2019-2022  Made to Order Software Corp.  All Rights Reserved
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
#include    "libexcept/demangle.h"


// C++
//
#include    <cxxabi.h>
#include    <memory>


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
 * \note
 * This is similar to using the c++filt command line tool. If the
 * conversion fails, then the function returns the input string as is.
 *
 * \note
 * I found a piece of code snippet on Catch2 which is used to demangle a C++
 * name. It is one simple ABI call!
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
 * \param[in] type_id_name  The mangled C++ name.
 *
 * \return The converted name.
 *
 * \sa https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
 */
std::string demangle_cpp_name(char const * type_id_name)
{
#if 1
    int status(0);

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(type_id_name, nullptr, nullptr, &status),
        std::free
    };

    return status == 0
                ? res.get()
                : type_id_name;
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
