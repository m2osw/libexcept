
# Introduction

The `libexcept` library provides a convenient class to derive your
exception classes from. It provides a stack trace capability.
At this point it is for Linux only.


# Usage

You just have to derive from this library exception classes to gain
access to the stack trace capability. Everything else will work under
the hood for you.

The stack trace capability can be reused by other functions if need
be. Especially, it is often used from within a Unix signal handler
(i.e. SEGV, BUS, FLT, etc.)

In most cases, the stack trace will tell you where the error occurred,
helping you focus on that function and finding the reason for the crash.


# Bugs

Submit bug reports and patches on
[github](https://github.com/m2osw/libexcept/issues).


_This file is part of the [snapcpp project](https://snapwebsites.org/)._
