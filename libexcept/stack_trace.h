// Copyright (c) 2011-2025  Made to Order Software Corp.  All Rights Reserved
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
#pragma once

// C++ includes
//
#include <string>
#include <list>


/** \file
 * \brief Declarations of the stack trace functions.
 *
 * This file defines the few functions, types, and default numbers in
 * link with the stack trace code.
 */


namespace libexcept
{


constexpr int const             STACK_TRACE_DEPTH = 20;

typedef std::list<std::string>  stack_trace_t;

stack_trace_t                   collect_stack_trace(int const stack_trace_depth
                                                        = STACK_TRACE_DEPTH);

stack_trace_t                   collect_stack_trace_with_line_numbers(
                                                    int const stack_trace_depth
                                                        = STACK_TRACE_DEPTH);


}
// namespace libexcept
// vim: ts=4 sw=4 et
