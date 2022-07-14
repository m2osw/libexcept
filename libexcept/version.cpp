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
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


// self
//
#include    "libexcept/version.h"


namespace libexcept
{



/** \brief Return a string to the version of the library when it was compiled.
 *
 * This function returns the library version when it was compiled as a string.
 * It can be used to compare against a version you support (i.e. the version
 * your software was compiled against could be different.)
 *
 * \return A constant string that represents the library version.
 */
char const * get_version_string()
{
    return LIBEXCEPT_VERSION_STRING;
}

/** \brief Return the major version number.
 *
 * This function returns the library major version number when it was compiled
 * as an integer. It can be used to compare against a version you support.
 *
 * \return The library major version number when it was compiled.
 */
int get_major_version()
{
    return LIBEXCEPT_VERSION_MAJOR;
}

/** \brief Return the minor version number.
 *
 * This function returns the library minor version number when it was compiled
 * as an integer. It can be used to compare against a version you support.
 *
 * \return The library minor version number when it was compiled.
 */
int get_minor_version()
{
    return LIBEXCEPT_VERSION_MINOR;
}

/** \brief Return the patch version number.
 *
 * This function returns the library patch version number when it was compiled
 * as an integer. It can be used to compare against a version you support.
 *
 * \return The library minor version number when it was compiled.
 */
int get_patch_version()
{
    return LIBEXCEPT_VERSION_PATCH;
}


} // namespace libexcept
// vim: ts=4 sw=4 et
