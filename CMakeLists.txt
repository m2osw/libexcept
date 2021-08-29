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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

cmake_minimum_required(VERSION 3.5)


project(libexcept)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC             TRUE)

## Include support modules
##
find_package(SnapCMakeModules REQUIRED)
find_package(Boost            REQUIRED)
find_package(SnapDoxygen              )

SnapGetVersion(LIBEXCEPT ${CMAKE_SOURCE_DIR})

## This will cause us to look in the 3rdParty area for thrift and other 3rdparty libs
##
link_directories(${CMAKE_BINARY_DIR}/dist/lib/)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

#option( RUN_TESTS "Enable CTest support and turn on the 'test' make target." OFF )
#if( ${RUN_TESTS} )
#    enable_testing()
#endif()

add_subdirectory(cmake)
add_subdirectory(libexcept)
add_subdirectory(tests)
add_subdirectory(doc)

# vim: ts=4 sw=4 et
