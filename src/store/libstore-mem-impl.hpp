/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store/libstore-mem-impl.hpp
 * Copyright (C) 2017-2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb is free software: you can redistribute it and/or
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

#ifndef _CYCLIC_LIBSTORE_MEM_IMPL_HPP_
#define _CYCLIC_LIBSTORE_MEM_IMPL_HPP_

#include "libstore.hpp"
#include "../common/common-file.hpp"

#include "libstore-base-impl.hpp"

#include <memory>
#include <string>
#include <vector>



namespace cyclic::store::impl
{

//
// Simple memory implementation of recordset
//
class memory_recordset_impl : public base_recordset_impl
{
protected:
    std::vector<raw_record> _data;

public:
    memory_recordset_impl() = delete;
    virtual ~memory_recordset_impl() = default;
    memory_recordset_impl(const memory_recordset_impl& other) = default;
    memory_recordset_impl(memory_recordset_impl&& other) = default;

    memory_recordset_impl(const std::vector<field_st>& fields, record_index_t first_index, record_time_t origin , record_time_t duration, const std::vector<raw_record>& data);
    memory_recordset_impl(const std::vector<field_st>& fields, record_index_t first_index, record_time_t origin , record_time_t duration, std::vector<raw_record>&& data);

    record_index_t record_count() const override;

    std::unique_ptr<record> get_record(record_index_t index) const override;

};

//
// Simple memory implementation of table
//

class memory_table_impl : public base_table_impl
{
protected:
    std::vector<raw_record> _data;

public:
    memory_table_impl() = default;
    virtual ~memory_table_impl() = default;
    /**
     * Create a cyclic table stored in memory.
     * This table is accessible for the current process only.
     * It will be alive until no client access it.
     * @param origin Table time origin.
     * @param duration Table time duration.
     * @param fields Field descriptors for create table.
     * @param record_capacity Table capacity in record number.
     * @throw std::invalid_argument Fields list is empty.
     * This is a non-sense to create a table without fields.
     * @throw std::invalid_argument Record capacity of 0.
     * This is a non-sense to create a table without storage capacity.
     * @throw std::invalid_argument Invalid record capacity.
     */
    virtual void create(const std::vector<field_st>& fields, record_index_t record_capacity,
        record_time_t origin =0, record_time_t duration =0) override;

    std::unique_ptr<recordset> get_recordset(record_index_t first, record_index_t last = record::absolute_max_index(), record_index_t count = std::numeric_limits<record_index_t>::max()) const override;

protected:
    raw_record get_record_at_position(record_index_t pos) const override;
    void reset_record_at_position(record_index_t pos) override;
    void set_record_at_position(record_index_t pos, const record& rec) override;
};

} // namespace cyclic::store::impl
#endif // _CYCLIC_LIBSTORE_MEM_IMPL_HPP_