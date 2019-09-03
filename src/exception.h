/*
 * Header:
 *      src/exception.h
 *
 * Description:
 *      The exception base class declarations.
 *
 * Documentation:
 *      See the corresponding .cpp file.
 *
 * License:
 *      Copyright (c) 2011-2019  Made to Order Software Corp.  All Rights Reserved
 * 
 *      https://snapwebsites.org/
 *      contact@m2osw.com
 * 
 *      Permission is hereby granted, free of charge, to any person obtaining a
 *      copy of this software and associated documentation files (the
 *      "Software"), to deal in the Software without restriction, including
 *      without limitation the rights to use, copy, modify, merge, publish,
 *      distribute, sublicense, and/or sell copies of the Software, and to
 *      permit persons to whom the Software is furnished to do so, subject to
 *      the following conditions:
 *
 *      The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 *      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once

// C++ includes
//
#include <stdexcept>
#include <string>
#include <vector>

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


constexpr int        STACK_TRACE_DEPTH = 20;

typedef std::vector<std::string>        stack_trace_t;

void                set_collect_stack( bool collect_stack );
stack_trace_t       collect_stack_trace( int const stack_trace_depth = STACK_TRACE_DEPTH );
stack_trace_t       collect_stack_trace_with_line_numbers( int const stack_trace_depth = STACK_TRACE_DEPTH );


class exception_base_t
{
public:
    explicit                    exception_base_t( int const stack_trace_depth = STACK_TRACE_DEPTH );

    virtual                     ~exception_base_t() {}

    stack_trace_t const &       get_stack_trace() const { return f_stack_trace; }

private:
    stack_trace_t               f_stack_trace = stack_trace_t();
};


class logic_exception_t
    : public std::logic_error
    , public exception_base_t
{
public:
    explicit                    logic_exception_t( std::string const & what, int const stack_trace_depth = STACK_TRACE_DEPTH );
    explicit                    logic_exception_t( char const *        what, int const stack_trace_depth = STACK_TRACE_DEPTH );

    virtual                     ~logic_exception_t() override {}

    virtual char const *        what() const throw() override;
};


class exception_t
    : public std::runtime_error
    , public exception_base_t
{
public:
    explicit                    exception_t( std::string const & what, int const stack_trace_depth = STACK_TRACE_DEPTH );
    explicit                    exception_t( char const *        what, int const stack_trace_depth = STACK_TRACE_DEPTH );

    virtual                     ~exception_t() override {}

    virtual char const *        what() const throw() override;
};


#define DECLARE_LOGIC_ERROR(name)                                       \
    class name : public ::libexcept::logic_exception_t {                \
    public: name(std::string const & msg) : logic_exception_t(#name ": " + msg) {} }

#define DECLARE_MAIN_EXCEPTION(name)                                    \
    class name : public ::libexcept::exception_t {                      \
    public: name(std::string const & msg) : exception_t(#name ": " + msg) {} }

#define DECLARE_EXCEPTION(base, name)                                   \
    class name : public base {                                          \
    public: name(std::string const & msg) : base(msg) {} }


}
// namespace libexcept
// vim: ts=4 sw=4 et
