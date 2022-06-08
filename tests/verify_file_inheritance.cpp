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

// libexcept
//
#include    "libexcept/file_inheritance.h"



/** \file
 * \brief A tool which verifies file inheritance.
 *
 * This tool is used to verify that we properly detect file inheritance.
 */


#include <string>
#include <iostream>
#include <unistd.h>

int main()
{
    libexcept::verify_inherited_files();
    return 0;
}


// vim: ts=4 sw=4 et
