// Copyright (c) 2011-2022  Made to Order Software Corp.  All Rights Reserved
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

// self
//
#include    <libexcept/stack_trace.h>


// C++ includes
//
#include    <map>
#include    <stdexcept>
#include    <string>
#include    <vector>


/** \file
 * \brief Declarations of the exception library.
 *
 * This file includes the library exception declarations.
 *
 * The strong point of the library is its ability to gather a stack
 * trace and attach that information to an exception.
 */


namespace libexcept
{



enum class collect_stack_t
{
    COLLECT_STACK_NO,           // no stack trace for exceptions
    COLLECT_STACK_YES,          // plain stack trace (fast)
    COLLECT_STACK_COMPLETE,     // include filenames & line numbers (slow)
};


typedef std::map<std::string, std::string>  parameter_t;


collect_stack_t     get_collect_stack();
void                set_collect_stack(collect_stack_t collect_stack);


class exception_base_t
{
public:
    explicit                    exception_base_t(int const stack_trace_depth = STACK_TRACE_DEPTH);

    virtual                     ~exception_base_t() {}

    parameter_t const &         get_parameters() const;
    std::string                 get_parameter(std::string const & name) const;
    void                        set_parameter(std::string const & name, std::string const & value);

    stack_trace_t const &       get_stack_trace() const { return f_stack_trace; }

private:
    parameter_t                 f_parameters = parameter_t();
    stack_trace_t               f_stack_trace = stack_trace_t();
};


class logic_exception_t
    : public std::logic_error
    , public exception_base_t
{
public:
    explicit                    logic_exception_t(std::string const & what, int const stack_trace_depth = STACK_TRACE_DEPTH);
    explicit                    logic_exception_t(char const *        what, int const stack_trace_depth = STACK_TRACE_DEPTH);

    virtual                     ~logic_exception_t() override {}

    virtual char const *        what() const throw() override;
};


class out_of_range_t
    : public std::out_of_range
    , public exception_base_t
{
public:
    explicit                    out_of_range_t(std::string const & what, int const stack_trace_depth = STACK_TRACE_DEPTH);
    explicit                    out_of_range_t(char const *        what, int const stack_trace_depth = STACK_TRACE_DEPTH);

    virtual                     ~out_of_range_t() override {}

    virtual char const *        what() const throw() override;
};


class exception_t
    : public std::runtime_error
    , public exception_base_t
{
public:
    explicit                    exception_t(std::string const & what, int const stack_trace_depth = STACK_TRACE_DEPTH);
    explicit                    exception_t(char const *        what, int const stack_trace_depth = STACK_TRACE_DEPTH);

    virtual                     ~exception_t() override {}

    virtual char const *        what() const throw() override;
};


#define DECLARE_LOGIC_ERROR(name)                                       \
    class name : public ::libexcept::logic_exception_t {                \
    public: name(std::string const & msg) : logic_exception_t(#name ": " + msg) {} }

#define DECLARE_OUT_OF_RANGE(name)                                      \
    class name : public ::libexcept::out_of_range_t {                   \
    public: name(std::string const & msg) : out_of_range_t(#name ": " + msg) {} }

#define DECLARE_MAIN_EXCEPTION(name)                                    \
    class name : public ::libexcept::exception_t {                      \
    public: name(std::string const & msg) : exception_t(#name ": " + msg) {} }

#define DECLARE_EXCEPTION(base, name)                                   \
    class name : public base {                                          \
    public: name(std::string const & msg) : base(msg) {} }


// a default logic error where I know there is a problem that needs to be
// fixed so that way we can quickly find the location & read the comments
//
DECLARE_LOGIC_ERROR(fixme);


}
// namespace libexcept
// vim: ts=4 sw=4 et
