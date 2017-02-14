/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common-file.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/libcyclicstore is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * cyclicdb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the COPYING file at the root of the source distribution for more details.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common-file.hpp"

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sstream>
#include <vector>

namespace cyclic
{
namespace io
{

//
// file
//

file::file(const file& file) :
_fd(::dup(file._fd))
{
}

file::file(file&& file) :
_fd(file._fd)
{
    file._fd = -1;
}

file& file::operator=(const file& file)
{
    _fd = ::dup(file._fd);
    return *this;
}

file& file::operator=(file&& file)
{
    _fd = file._fd;
    file._fd = -1;
    return *this;
}

file::~file() /* throw (io_exception) */
{
    close();
}

void file::open(const std::string& path)/*throw (io_exception)*/
{
    int fd = ::open(path.c_str(), O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
    if(fd != -1)
    {
        _fd = fd;
    }
    else
    {
        throw io_exception(errno);
    }
}

void file::create(const std::string& path)/*throw (io_exception)*/
{
    int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    if(fd != -1)
    {
        _fd = fd;
    }
    else
    {
        throw io_exception(errno);
    }
}

file& file::write(const void* buff, size_t size) /*throw (io_exception)*/
{
    ssize_t res = ::write(_fd, buff, size);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only write " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::write_at(const void* buff, size_t size, size_t offset) /*throw (io_exception)*/
{
    ssize_t res = ::pwrite(_fd, buff, size, (off_t) offset);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only write " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::write_n(uint8_t c, size_t size) /*throw (io_exception)*/
{
    std::vector<uint8_t> buff(size, c);
    ssize_t res = ::write(_fd, buff.data(), size);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only write " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::write_n_at(uint8_t c, size_t size, size_t offset) /*throw (io_exception)*/
{
    std::vector<uint8_t> buff(size, c);
    ssize_t res = ::pwrite(_fd, buff.data(), size, (off_t) offset);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only write " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::read(void* buff, size_t size) /*throw (io_exception)*/
{
    ssize_t res = ::read(_fd, buff, size);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only read " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::read_at(void* buff, size_t size, size_t offset) /*throw (io_exception)*/
{
    ssize_t res = ::pread(_fd, buff, size, (off_t) offset);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    if(res != size)
    {
        std::stringstream stm;
        stm << "Only read " << res << " / " << size << " byte(s)";
        throw io_exception{stm.str()};
    }
    return *this;
}

file& file::seek(size_t offset) /*throw (io_exception)*/
{
    off_t res = ::lseek(_fd, offset, SEEK_SET);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    return *this;
}

file& file::sync() /*throw (io_exception)*/
{
    int res = ::fsync(_fd);
    if(res == -1)
    {
        throw io_exception(errno);
    }
    return *this;
}

void file::close() /*throw (io_exception)*/
{
    if(_fd != -1)
    {
        int res = ::close(_fd);
        if(res == -1)
        {
            throw io_exception(errno);
        }
        _fd = -1;
    }
}

bool file::ok()const
{
    return _fd != -1;
}

file::operator bool()const
{
    return ok();
}

void file::remove(const std::string& path)/*throw (io_exception)*/
{
    if(::remove(path.c_str()) == -1)
    {
        throw io_exception(errno);
    }
}

}
} // namespace cyclic::io