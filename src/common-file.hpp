/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common-file.hpp
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

#ifndef _CYCLIC_COMMON_FILE_HPP_
#define _CYCLIC_COMMON_FILE_HPP_

#include <memory>
#include <stdexcept>
#include <system_error>
#include <string>

namespace cyclic
{
/**
 * IO tools.
 */
namespace io
{

/**
 * Exception thrown on I/O error.
 */
class io_exception : public std::system_error
{
public:

    explicit io_exception(int err):
        system_error(err, std::generic_category())
    {}

    explicit io_exception(int err, const std::string& what_arg):
        system_error(err, std::generic_category(), what_arg)
    {}

    explicit io_exception(int err, const char* what_arg):
        system_error(err, std::generic_category(), what_arg)
    {}

    explicit io_exception(const std::string& what_arg):
        io_exception(0, what_arg)
    {}

    explicit io_exception(const char* what_arg):
        io_exception(0, what_arg)
    {}

    virtual ~io_exception() = default;
};

/**
 * Helper for file i/o.
 */
class file
{
public:
    file() = default;
    file(const file& file);
    file(file&& file);
    file& operator=(const file& file);
    file& operator=(file&& file);
    virtual ~file() /* throw(io_exception) */;

    void open(const std::string& path) /*throw (io_exception)*/;
    void create(const std::string& path) /*throw (io_exception)*/;

    file& write(const void* buff, size_t size) /*throw (io_exception)*/;
    file& write_at(const void* buff, size_t size, size_t offeset) /*throw (io_exception)*/;
    file& write_n(uint8_t c, size_t size) /*throw (io_exception)*/;
    file& write_n_at(uint8_t c, size_t size, size_t offset) /*throw (io_exception)*/;
    file& read(void* buff, size_t size) /*throw (io_exception)*/;
    file& read_at(void* buff, size_t size, size_t offset) /*throw (io_exception)*/;
    file& seek(size_t offset) /*throw (io_exception)*/;
    file& sync() /*throw (io_exception)*/;

    void close() /*throw (io_exception)*/;

    bool ok()const;
    operator bool()const;

    template<typename T> file& write(const T& value) /*throw (io_exception)*/;
    template<typename T> file& write_at(const T& value, size_t offset) /*throw (io_exception)*/;
    template<typename T> file& read(T& value) /*throw (io_exception)*/;
    template<typename T> file& read_at(T& value, size_t offset) /*throw (io_exception)*/;

    template<size_t sz>
    file& skip() /*throw (io_exception)*/;


    static void remove(const std::string& path)/*throw (io_exception)*/;

protected:
    int _fd = -1;

    file(int fd) : _fd(fd)
    {
    }

};

template<typename T>
file& file::write(const T& value) /*throw (io_exception)*/
{
    return write(&value, sizeof (T));
}

template<typename T>
file& file::write_at(const T&value, size_t offset) /*throw (io_exception)*/
{
    return write(&value, sizeof (T), offset);
}

template<typename T>
file& file::read(T&value) /*throw (io_exception)*/
{
    return read(&value, sizeof (T));
}

template<typename T>
file& file::read_at(T&value, size_t offset) /*throw (io_exception)*/
{
    return read(&value, sizeof (T), offset);
}

template<size_t sz>
file& file::skip() /*throw (io_exception)*/
{
    uint8_t buffer[sz];
    return read(buffer, sz);
}

}
} // namespace cyclic::io
#endif // _CYCLIC_COMMON_FILE_HPP_