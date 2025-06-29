// Copyright (c) 2012-2025  Made to Order Software Corp.  All Rights Reserved
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
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// self
//
#include    "catch_main.h"


// libexcept
//
#include    <libexcept/exception.h>





namespace
{





}


CATCH_TEST_CASE("trace_mode", "[trace][exception]")
{
    CATCH_START_SECTION("trace mode")
    {
        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_YES);

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("user_exception", "[trace][exception]")
{
    CATCH_START_SECTION("logic exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        DECLARE_LOGIC_ERROR(test_logic_exception);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            test_logic_exception e(test_logic_exception(
                          "this is the what message"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_logic_exception: this is the what message") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            test_logic_exception e(test_logic_exception(
                          "try with \"yes\" for the stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_logic_exception: try with \"yes\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            test_logic_exception e(test_logic_exception(
                          "try with \"complete\" for the stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_logic_exception: try with \"complete\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("range exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        DECLARE_OUT_OF_RANGE(test_range_exception);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            test_range_exception e(test_range_exception(
                          "range error"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_range_exception: range error") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            test_range_exception e(test_range_exception(
                          "range error \"yes\" basic stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_range_exception: range error \"yes\" basic stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            test_range_exception e(test_range_exception(
                          "range error \"complete\" complete stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "test_range_exception: range error \"complete\" complete stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("run-time exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        DECLARE_MAIN_EXCEPTION(run_time_exception);
        DECLARE_EXCEPTION(run_time_exception, test_run_time_exception);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            test_run_time_exception e(test_run_time_exception(
                          "run time error"
                    ));

            CATCH_CHECK(strcmp(e.what(), "run_time_exception: run time error") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            test_run_time_exception e(test_run_time_exception(
                          "range error \"yes\" basic stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "run_time_exception: range error \"yes\" basic stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            test_run_time_exception e(test_run_time_exception(
                          "range error \"complete\" complete stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "run_time_exception: range error \"complete\" complete stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("direct_exception", "[trace][exception]")
{
    CATCH_START_SECTION("direct logic exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            libexcept::logic_exception_t e(libexcept::logic_exception_t(
                          "direct logic exception"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct logic exception") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            libexcept::logic_exception_t e(libexcept::logic_exception_t(
                          "direct logic exception with \"yes\" for the stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct logic exception with \"yes\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            libexcept::logic_exception_t e(libexcept::logic_exception_t(
                          "direct logic exception with \"yes\" for the stack"
                        , 3
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct logic exception with \"yes\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 3);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            libexcept::logic_exception_t e(libexcept::logic_exception_t(
                          "direct logic exception with \"complete\" for the stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct logic exception with \"complete\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            libexcept::logic_exception_t e(libexcept::logic_exception_t(
                          "direct logic exception with \"complete\" for the stack"
                        , 2
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct logic exception with \"complete\" for the stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 2);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("direct range exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            libexcept::out_of_range_t e(libexcept::out_of_range_t(
                          "direct range error"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            libexcept::out_of_range_t e(libexcept::out_of_range_t(
                          "direct range error \"yes\" basic stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"yes\" basic stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            libexcept::out_of_range_t e(libexcept::out_of_range_t(
                          "direct range error \"complete\" complete stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"complete\" complete stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("direct run-time exception")
    {
        CATCH_CHECK(libexcept::get_collect_stack() == libexcept::collect_stack_t::COLLECT_STACK_NO);

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
            libexcept::exception_t e(libexcept::exception_t(
                          "direct run time error"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct run time error") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            libexcept::exception_t e(libexcept::exception_t(
                          "direct range error \"yes\" basic stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"yes\" basic stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_YES);
            libexcept::exception_t e(libexcept::exception_t(
                          "direct range error \"yes\" basic stack"
                        , 0
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"yes\" basic stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 0);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            libexcept::exception_t e(libexcept::exception_t(
                          "direct range error \"complete\" complete stack"
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"complete\" complete stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() > 0);
            CATCH_CHECK(stack.size() < libexcept::STACK_TRACE_DEPTH);
        }

        {
            libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_COMPLETE);
            libexcept::exception_t e(libexcept::exception_t(
                          "direct range error \"complete\" complete stack"
                        , 2
                    ));

            CATCH_CHECK(strcmp(e.what(), "direct range error \"complete\" complete stack") == 0);

            libexcept::stack_trace_t const stack(e.get_stack_trace());
            CATCH_CHECK(stack.size() == 2);
        }

        libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO);
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("exception_parameters", "[parameters][exception]")
{
    CATCH_START_SECTION("exception parameters")
    {
        libexcept::exception_t basic_exception("something bad happened");

        CATCH_CHECK(strcmp(basic_exception.what(), "something bad happened") == 0);

        basic_exception.set_parameter("filename", "/etc/aliases");
        basic_exception.set_parameter(std::string(), "ignored");

        CATCH_CHECK(basic_exception.get_parameter("filename") == "/etc/aliases");
        CATCH_CHECK(basic_exception.get_parameter("undefined").empty());
        CATCH_CHECK(basic_exception.get_parameter(std::string()).empty());
        CATCH_CHECK(basic_exception.get_parameters().size() == 1);

        try
        {
            throw basic_exception;
        }
        catch(libexcept::exception_t const & e)
        {
            CATCH_CHECK(strcmp(e.what(), "something bad happened") == 0);
            CATCH_CHECK(e.get_parameter("filename") == "/etc/aliases");
            CATCH_CHECK(e.get_parameter("undefined").empty());
            CATCH_CHECK(e.get_parameter(std::string()).empty());
            CATCH_CHECK(e.get_parameters().size() == 1);
        }
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
