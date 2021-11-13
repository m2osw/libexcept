# Copyright (c) 2011-2021  Made to Order Software Corp.  All Rights Reserved
#
# https://snapwebsites.org/project/libexcept
# contact@m2osw.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

project(except)

configure_file(
    version.h.in
    ${PROJECT_BINARY_DIR}/version.h
)

add_library(${PROJECT_NAME} SHARED
    demangle.cpp
    exception.cpp
    stack_trace.cpp
    version.cpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        VERSION
            ${LIBEXCEPT_VERSION_MAJOR}.${LIBEXCEPT_VERSION_MINOR}

        SOVERSION
            ${LIBEXCEPT_VERSION_MAJOR}
)

install(
    TARGETS
        ${PROJECT_NAME}

    LIBRARY DESTINATION
        lib
)

install(
    FILES
        demangle.h
        exception.h
        stack_trace.h
        ${PROJECT_BINARY_DIR}/version.h

    DESTINATION
        include/libexcept
)

# vim: ts=4 sw=4 et
