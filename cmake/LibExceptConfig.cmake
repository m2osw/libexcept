# - Find LibExcept
#
# LIBEXCEPT_FOUND        - System has LibExcept
# LIBEXCEPT_INCLUDE_DIRS - The LibExcept include directories
# LIBEXCEPT_LIBRARIES    - The libraries needed to use LibExcept
# LIBEXCEPT_DEFINITIONS  - Compiler switches required for using LibExcept
#
# License:
#
# Copyright (c) 2011-2022  Made to Order Software Corp.  All Rights Reserved
#
# https://snapwebsites.org/project/libexcept
# contact@m2osw.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

find_path(
    LIBEXCEPT_INCLUDE_DIR
        libexcept/exception.h

    PATHS
        ENV LIBEXCEPT_INCLUDE_DIR
)

find_library(
    LIBEXCEPT_LIBRARY
        except

    PATHS
        ${LIBEXCEPT_LIBRARY_DIR}
        ENV LIBEXCEPT_LIBRARY
)

mark_as_advanced(
    LIBEXCEPT_INCLUDE_DIR
    LIBEXCEPT_LIBRARY
)

set(LIBEXCEPT_INCLUDE_DIRS ${LIBEXCEPT_INCLUDE_DIR})
set(LIBEXCEPT_LIBRARIES    ${LIBEXCEPT_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    LibExcept
    REQUIRED_VARS
        LIBEXCEPT_INCLUDE_DIR
        LIBEXCEPT_LIBRARY
)

# vim: ts=4 sw=4 et
