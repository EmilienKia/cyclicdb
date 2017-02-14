/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore-file-impl.hpp
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

#ifndef _CYCLIC_LIBSTORE_FILE_IMPL_HPP_
#define _CYCLIC_LIBSTORE_FILE_IMPL_HPP_

#include "libstore.hpp"
#include "common-file.hpp"

#include "libstore-base-impl.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cyclic
{
namespace store
{
namespace impl
{

//
// Simple file implementation of table
//

class file_table_impl : public base_table_impl
{
protected:
    static std::string FILE_MARKER;

    std::string _filename;
    mutable io::file    _file;

    /** File default current version. */
    const char _version_marker[2] {'0', '1'};
    uint16_t _global_options;

    uint32_t _table_header_size = 8 + 40 + 32; // See file spec
    uint32_t _table_size;

    uint32_t _record_options;
    uint32_t _record_header_size;
    uint32_t _record_size;

    // _field_details // TODO

    static constexpr uint32_t _table_index_descriptor_position = 48; // See file spec

public:
    file_table_impl() = default;
    virtual ~file_table_impl();

    /**
     * Create a file table storage.
     * @param filename Name of file to create to store table.
     * @param fields Field descriptors for create table.
     * @param record_capacity Table capacity in record number.
     * @param origin Table time origin.
     * @param duration Table time duration.
     * @throw std::invalid_argument Fields list is empty.
     * This is a non-sense to create a table without fields.
     * @throw std::invalid_argument Record capacity of 0.
     * This is a non-sense to create a table without storage capacity.
     * @throw std::invalid_argument Invalid record capacity.
     * @throw cyclic::io::io_exception An I/O exception occurs.
     */
    void create(const std::string& filename, const std::vector<field_st>& fields,
        record_index_t record_capacity, record_time_t origin, record_time_t duration);

    /**
     * Open a table from a file.
     * @param filename Name of table file to open.
     * @throw std::invalid_argument Filename shall be specified.
     * @throw cyclic::io::io_exception An I/O exception occurs.
     */
    void open(const std::string& filename, const io::file& file, const std::string& version);

protected:
    static uint16_t field_size(data_type type);

    void initialize_on_creation(const std::vector<field_st>& fields);
    void create_table_file();

    void read_table_index_descriptor();
    void write_table_index_descriptor();

    raw_record get_record_at_position(record_index_t pos) const override;
    void reset_record_at_position(record_index_t pos) override;
    void set_record_at_position(record_index_t pos, const record& rec) override;
};

}}} // namespace cyclic::store::impl
#endif // _CYCLIC_LIBSTORE_FILE_IMPL_HPP_