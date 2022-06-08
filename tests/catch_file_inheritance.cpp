// Copyright (c) 2012-2022  Made to Order Software Corp.  All Rights Reserved
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// libexcept
//
#include    <libexcept/file_inheritance.h>


// self
//
#include    "catch_main.h"


// C++
//
#include    <fstream>



CATCH_TEST_CASE("file_inheritance", "[file_inheritance]")
{
    std::string path("../../BUILD/Debug/contrib/libexcept/tests/verify-file-inheritance");

    CATCH_START_SECTION("file_inheritance: verify that a process succeed in a clean environment")
    {
        int const r(system(path.c_str()));
        CATCH_REQUIRE(r == 0);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("file_inheritance: verify that a process fails if unexpected files are inherited")
    {
        // create a file and keep it open when we call system()
        //
        std::ofstream out(SNAP_CATCH2_NAMESPACE::g_tmp_dir() + "/file-to-inherit.txt");
        out << "Test" << std::endl;

        int const r(system(path.c_str()));
        CATCH_REQUIRE(r != 0);
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
