
<p align="center">
<img alt="libexcept" title="libexcept -- a library to get verbose exceptions."
src="https://snapwebsites.org/sites/snapwebsites.org/files/images/libexcept-logo.png" width="200" height="200"/>
</p>

# Introduction

The `libexcept` library provides a convenient class to derive your
exception classes from. It provides a stack trace capability.
At this point it is for Linux only.


# Usage

In your code, you just have to derive from this library exception classes
to gain access to the stack trace capability. Everything else will work
under the hood for you.

The stack trace capability can be reused by other functions if needed.
Especially, it is often used from within a Unix signal handler
(i.e. SEGV, BUS, FLT, etc.)

In most cases, the stack trace tells you where the error occurred,
helping you focus on that function and finding the reason for the crash.


# Requirements

There is one really important requirements on Linux. You must compile your
application with the `-rdynamic` option. It may make the binary larger and
start the application a little slower, but it should not otherwise impair
the rest of the code.


# Bugs

Submit bug reports and patches on
[github](https://github.com/m2osw/libexcept/issues).


_This file is part of the [snapcpp project](https://snapwebsites.org/)._
