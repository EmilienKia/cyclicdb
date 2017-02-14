/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore.cpp
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

#include "libstore.hpp"

#include "libstore-base-impl.hpp"
#include "libstore-file-impl.hpp"
#include "libstore-mem-impl.hpp"

#include <iostream>
#include <sstream>
#include <limits>

namespace cyclic
{
namespace store
{
//
// memory
//

std::unique_ptr<cyclic::table> memory::create(const std::vector<field_st>& fields, record_index_t record_capacity,
        record_time_t origin, record_time_t duration)
{
    std::unique_ptr<impl::memory_table_impl> tbl(new impl::memory_table_impl);
    tbl->create(fields, record_capacity, origin, duration);
    return tbl;
}

//
// file
//

std::unique_ptr<cyclic::table> file::create(const std::string& filename, table_type type,
        const std::vector<field_st>& fields, record_index_t record_capacity,
        record_time_t origin, record_time_t duration)
{
    std::unique_ptr<impl::file_table_impl> tbl(new impl::file_table_impl);
    tbl->create(filename, fields, record_capacity, origin, duration);
    return tbl;
}

std::unique_ptr<cyclic::table> file::open(const std::string& filename)
{
    if(filename.empty())
    {
        // File name must be provided.
        throw std::invalid_argument{"Filename shall be specified"};
    }

    io::file file;
    file.open(filename);
    if(!file)
    {
        // Handle file problem.
        std::ostringstream stm;
        stm << "Error while opening table file " << filename << std::endl;
        throw cyclic::io::io_exception(0, stm.str());
    }

    std::string marker(4, ' ');
    file.read((char*) marker.data(), 4);
    if(marker != "CYDB")
    {
        // Handle bad file format.
        throw cyclic::io::io_exception(0, "Unsupported file format");
    }

    std::string version(2, ' ');
    file.read((char*) version.data(), 2);
    if(version != "01")
    {
        // Handle bad file version.
        std::ostringstream stm;
        stm << "Unsupported file version " << version << std::endl;
        throw cyclic::io::io_exception(0, stm.str());
    }

    std::unique_ptr<impl::file_table_impl> tbl(new impl::file_table_impl);
    tbl->open(filename, file, version);
    return tbl;
}

}} // namespace cyclic