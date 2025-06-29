// Copyright (c) 2012-2024  Made to Order Software Corp.  All Rights Reserved
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
#include    "catch_main.h"


// libexcept
//
#include    <libexcept/exception.h>
#include    <libexcept/demangle.h>


// C
//
//#include    <unistd.h>


namespace
{


class object
{
public:
    object(int count)
        : f_count(count)
    {
    }

    std::string demangle1()
    {
        return libexcept::demangle_cpp_name(typeid(std::string (*)()).name());
    }

    std::string demangle2()
    {
        return libexcept::demangle_cpp_name(typeid(long (*)(std::string const & filename, bool temporary, short int power)).name());
    }

private:
    int     f_count = 0;
};



}


CATCH_TEST_CASE("demangle", "[demangle]")
{
    CATCH_START_SECTION("demangle")
    {
        object o(5);

        CATCH_CHECK(o.demangle1() == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > (*)()");
        CATCH_CHECK(o.demangle2() == "long (*)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, bool, short)");
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
