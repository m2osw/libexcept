// Copyright (c) 2011-2022  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/
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

// self
//
#include    "./file_inheritance.h"


// C++ includes
//
#include    <fstream>
#include    <iostream>
#include    <list>
#include    <memory>


// C lib includes
//
#include    <dirent.h>
#include    <string.h>
#include    <unistd.h>


/** \file
 * \brief Implementation of a test of the files inherited.
 *
 * Most processes are expected to only inherit stdin, stdout, and stderr.
 * This mimplementation verifies that this is the case.
 */



namespace libexcept
{



/** \brief Load the command line of the specified process.
 *
 * This function loads the cmdline file of the specified process. If an
 * error occurs, the function returns an empty string. Some processes do
 * not have a command line.
 *
 * If your program can include cppprocess (see eventdispatcher), then I
 * suggest you use the cppprocess::process_info class instead. It is
 * much more powerful.
 *
 * \param[in] pid  The process identifier.
 *
 * \return The command line of the \p pid process.
 */
std::string get_command_line(pid_t pid)
{
    std::string filename("/proc/");
    filename += std::to_string(pid);
    filename += "/cmdline";
    std::ifstream cmdline(filename);
    std::string line;
    std::getline(cmdline, line, '\0');
    return line;
}

/** \brief Check the list of files opened in this process.
 *
 * This function reads the /proc/<pid>/fd directory. If it finds files other
 * than 0, 1, 2, then it generates an error.
 *
 * In Debug mode, the error is fatal (it throws).
 *
 * In Release mode, the error is _just_ a warning so the process still starts
 * but you still get a message letting you know there may be something fishy
 * going on.
 *
 * In case your application actually accepts additional streams, you can
 * add them to the \p allowed set of file descriptors.
 *
 * \param[in] allowed  Additional allowed input streams.
 */
void verify_inherited_files(allowed_fds_t allowed)
{
    auto closedir = [](DIR * d)
    {
        ::closedir(d);
    };

    int errcnt(0);

    std::string path("/proc/");
    path += std::to_string(getpid());
    path += "/fd";
    DIR * d(opendir(path.c_str()));
    std::unique_ptr<DIR, decltype(closedir)> auto_close(d, closedir);

    for(;;)
    {
        dirent const * ent(readdir(d));
        if(ent == nullptr)
        {
            break;
        }

        char const * basename(ent->d_name);
        if(basename[0] == '.')
        {
            // ignore all hidden files
            continue;
        }

        if(basename[0] >= '0'
        && basename[0] <= '2'
        && basename[1] == '\0')
        {
            // skip stdin, stdout, stderr
            continue;
        }

        int const fd(std::atoi(basename));
        if(fd == dirfd(d))
        {
            // ignore the opendir() file descriptor
            // (it is currently open since we are reading the directory)
            //
            continue;
        }

        if(allowed.find(fd) == allowed.end())
        {
            char link[256];
            ssize_t const l(readlink((path + '/' + basename).c_str(), link, sizeof(link) - 1));
            if(l <= 0)
            {
                link[0] = '\0';
            }
            else
            {
                link[l] = '\0';
            }
            std::cerr
                << "warning: file descriptor "
                << fd
                << " ("
                << link
                << ") leaked on invocation. Parent PID "
                << getppid()
                << ": "
                << get_command_line(getppid())
                << '\n';
            ++errcnt;
        }
    }

#ifdef _DEBUG
    if(errcnt > 0)
    {
        throw std::runtime_error("found unexpected file descriptor leaks.");
    }
#endif
}



}
// namespace libexcept
// vim: ts=4 sw=4 et
