# Copyright (c) 2011-2021  Made to Order Software Corp.  All Rights Reserved
#
# https://snapwebsites.org/
# contact@m2osw.com
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

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

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${BOOST_INCLUDE_DIRS}
)

#target_link_libraries(${PROJECT_NAME}
#    dl
#)

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