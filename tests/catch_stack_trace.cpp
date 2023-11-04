// Copyright (c) 2012-2023  Made to Order Software Corp.  All Rights Reserved
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


// eventdispatcher lib
//
#include    <libexcept/exception.h>





namespace
{



class object
{
public:
    object(int count)
        : f_count(count)
    {
    }

    libexcept::stack_trace_t direct_stack_trace();

    int get_line() const
    {
        return f_line;
    }

private:
    int     f_count = 0;
    int     f_line = 0;
};



libexcept::stack_trace_t object::direct_stack_trace()
{
    f_line = __LINE__;
    return libexcept::collect_stack_trace_with_line_numbers(f_count);
}



}


CATCH_TEST_CASE("stack_trace", "[trace]")
{
    CATCH_START_SECTION("stack trace")
    {
        object o(5);
        libexcept::stack_trace_t stack(o.direct_stack_trace());
        int const local_line(__LINE__);

        CATCH_CHECK(stack.size() == 5);

        // in the coverage test (and if you're testing with the sanitizer
        // version) then we have an extra entry for the sanitizer
        //
        auto s(stack.begin());
        if(s->find("sanitizer") != std::string::npos)
        {
            ++s;
        }

//{
//for(auto l : stack)
//{
//std::cerr << "--- " << l << "\n";
//}
//}

        // first line is inside the libexcept function
        //
        CATCH_CHECK(s->find("stack_trace.cpp") != std::string::npos);

        // second line is here in our test, the direct_stack_trace() function
        //
        ++s;
        std::string::size_type pos(s->find("catch_stack_trace.cpp"));
        CATCH_CHECK(pos != std::string::npos);
        std::string line_number(s->substr(pos + 21 + 1));     // +1 to skip the ':'
        int line(atoi(line_number.c_str()));
        //bool correct_line(line == o.get_line() + 1
        //                || line == o.get_line() + 2);
        CATCH_CHECK((line == o.get_line() + 1
                        || line == o.get_line() + 2));

        // third line is in this very function
        //
        ++s;
        pos = s->find("catch_stack_trace.cpp");
        CATCH_CHECK(pos != std::string::npos);
        line_number = s->substr(pos + 21 + 1);     // +1 to skip the ':'
        line = atoi(line_number.c_str());
        CATCH_CHECK((line == local_line
                    || line == local_line - 1));

        // further lines are caller's info... that could change on a whimp
        // so we ignore them; but we already proved that our addr2line worked
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
