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


void tokenize(std::list<std::string> list, std::string const & p)
{
    char const * s(p.c_str());
    while(*s != '\0')
    {
        char const * e(s);
        while(*e != '\0' && *e != '/')
        {
            ++e;
        }
        if(s < e)
        {
            list.push_back(std::string(s, e - s));
        }
        s = e;
    }
}

std::string join_strings(std::list<std::string> list, char sep)
{
    std::string result;
    for(auto const & l : list)
    {
        if(!result.empty())
        {
            result += sep;
        }
        result += l;
    }
    return result;
}


CATCH_TEST_CASE("file_inheritance", "[file_inheritance]")
{
    std::string path(SNAP_CATCH2_NAMESPACE::g_verify_file_inheriance_path);
    path += "/verify-file-inheritance";

    CATCH_START_SECTION("file_inheritance: check command line")
    {
        std::string cmd(libexcept::get_command_line(getpid()));
        char * p(realpath(cmd.c_str(), nullptr));
        CATCH_REQUIRE(p != nullptr);

        // this changes depending on whether we run the test directly
        // or from within coverage, so I check the paths with segments
        // allowing various levels on either side
        //
        std::list<std::string> cmd_seg;
        tokenize(cmd_seg, p);

        // in the normal testing, we have a full filename to the unittest
        // in the binary tree
        //
        // in case of the coverage test, we just have "BUILD/tests/unittest"
        //
        std::string const expected(SNAP_CATCH2_NAMESPACE::g_verify_file_inheriance_path + "/unittest");
        std::list<std::string> expected_seg;
        tokenize(expected_seg, expected);

        auto cmd_it(cmd_seg.begin());
        auto expected_it(expected_seg.begin());
        while(cmd_it != cmd_seg.end()
           && expected_it != expected_seg.end())
        {
            if(*cmd_it == *expected_it)
            {
                ++cmd_it;
                ++expected_it;
            }
            else if(*cmd_it == "BUILD")
            {
                for(;
                    expected_it != expected_seg.end() && *expected_it != "BUILD";
                    ++expected_it);
            }
            else if(*cmd_it == "coverage")
            {
                ++cmd_it;
                CATCH_REQUIRE(cmd_it != cmd_seg.end());
                CATCH_REQUIRE(*cmd_it == "BUILD");
                ++cmd_it;
            }
            else
            {
                // not a match
                break;
            }
        }
        if(cmd_it != cmd_seg.end()
        || expected_it != expected_seg.end())
        {
            std::list<std::string> rem(cmd_it, cmd_seg.end());
            if(!rem.empty())
            {
                std::cerr << "error: cmd_it still has: /"
                    << join_strings(rem, '/')
                    << " from "
                    << cmd
                    << '\n';
            }

            rem = std::list<std::string>(expected_it, expected_seg.end());
            if(!rem.empty())
            {
                std::cerr << "error: expected_it still has: "
                    << join_strings(rem, '/')
                    << " from "
                    << expected
                    << '\n';
            }

            CATCH_REQUIRE(!"cmd_it != expected_it");
        }

        free(p);
    }
    CATCH_END_SECTION()

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
