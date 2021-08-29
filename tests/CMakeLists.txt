# Copyright (c) 2021  Made to Order Software Corp.  All Rights Reserved
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


##
## libexcept unittest
##
project(unittest)

find_package(Catch2)

if(Catch2_FOUND)

    add_definitions(
        -DCATCH_CONFIG_PREFIX_ALL
    )

    add_executable(${PROJECT_NAME}
        catch_main.cpp

        catch_demangle.cpp
        catch_exceptions.cpp
        catch_stack_trace.cpp
        catch_version.cpp
    )

    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_BINARY_DIR}
            ${PROJECT_SOURCE_DIR}/..
            ${CATCH_INCLUDE_DIR}
    )

    target_link_libraries(${PROJECT_NAME}
        except
    )

    add_test(unittests ${PROJECT_NAME})

else(Catch2_FOUND)

    message("snapcatch2 not found... no test will be built.")

endif(Catch2_FOUND)

# vim: ts=4 sw=4 et