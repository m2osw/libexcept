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
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// Tell catch we want it to add the runner code in this file.
#define CATCH_CONFIG_RUNNER

// self
//
#include    "catch_main.h"


// libexcept lib
//
#include    <libexcept/exception.h>
#include    <libexcept/version.h>


// C++ lib
//
#include    <sstream>



namespace SNAP_CATCH2_NAMESPACE
{


std::string  g_verify_file_inheriance_path;


} // namespace SNAP_CATCH2_NAMESPACE


namespace
{


Catch::Clara::Parser add_command_line_options(Catch::Clara::Parser const & cli)
{
    return cli
         | Catch::Clara::Opt(SNAP_CATCH2_NAMESPACE::g_verify_file_inheriance_path, "verify_file_inheritance_path")
              ["--verify-file-inheritance"]
              ("supply a path to the verify-file-inheritance executable used to test that file leaks are detected");
}


} // no name namespace


int main(int argc, char * argv[])
{
    return SNAP_CATCH2_NAMESPACE::snap_catch2_main(
              "libexcept"
            , libexcept::LIBEXCEPT_VERSION_STRING
            , argc
            , argv
            , []() { libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO); }
            , &add_command_line_options
        );
}


// vim: ts=4 sw=4 et
