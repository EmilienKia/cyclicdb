/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore-base-impl.hpp
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

#ifndef _CYCLIC_LIBSTORE_BASE_IMPL_HPP_
#define _CYCLIC_LIBSTORE_BASE_IMPL_HPP_

#include "libstore.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace cyclic
{
namespace store
{
/**
 * Implementation of CyclicDB storage layer.
 */
namespace impl
{

class base_table_impl;
struct table_impl_state;
struct table_impl_state_no_record;
struct table_impl_state_one_record_at_begining;
struct table_impl_state_one_record_somewhere;
struct table_impl_state_one_record_at_end;
struct table_impl_state_partial_contiguous_at_begining;
struct table_impl_state_full_contiguous;
struct table_impl_state_partial_contiguous_somewhere;
struct table_impl_state_partial_contiguous_at_end;
struct table_impl_state_partial_split_somewhere;
struct table_impl_state_partial_split_at_end;
struct table_impl_state_full_split_somewhere;
struct table_impl_state_full_split_at_end;

enum TABLE_STATE
{
    TABLE_NO_RECORD = 0,
    TABLE_ONE_RECORD_AT_BEGINING,
    TABLE_ONE_RECORD_SOMEWHERE,
    TABLE_ONE_RECORD_AT_END,
    TABLE_PARTIAL_CONTIGUOUS_AT_BEGINING,
    TABLE_FULL_CONTIGUOUS,
    TABLE_PARTIAL_CONTIGUOUS_SOMEWHERE,
    TABLE_PARTIAL_CONTIGUOUS_AT_END,
    TABLE_PARTIAL_SPLIT_SOMEWHERE,
    TABLE_PARTIAL_SPLIT_AT_END,
    TABLE_FULL_SPLIT_SOMEWHERE,
    TABLE_FULL_SPLIT_AT_END
};

//
// Simple implementation of field descriptor
//
class field_impl : public field
{
    friend class file_table_impl;
protected:
    data_type _type = CDB_DT_UNSPECIFIED;
    field_index_t _index = field::invalid_index();
    std::string _name;

    uint16_t _size, _offset;;
public:
    field_impl();
    field_impl(const field_impl& field);
    field_impl(field_impl&& field);
    field_impl(data_type type, field_index_t index, const std::string& name, uint16_t size = 0, uint16_t offset = 0);

    field_impl& operator=(const field_impl& field);
    field_impl& operator=(field_impl&& field);

    virtual field_index_t index() const;
    virtual std::string name() const;
    virtual data_type type() const;

    virtual uint16_t size() const;
    virtual uint16_t offset() const;

};
/**
 * Base of simple implementation of table
 */

class base_table_impl : public table
{
    friend struct table_impl_state;
    friend struct table_impl_state_no_record;
    friend struct table_impl_state_one_record_at_begining;
    friend struct table_impl_state_one_record_somewhere;
    friend struct table_impl_state_one_record_at_end;
    friend struct table_impl_state_partial_contiguous_at_begining;
    friend struct table_impl_state_full_contiguous;
    friend struct table_impl_state_partial_contiguous_somewhere;
    friend struct table_impl_state_partial_contiguous_at_end;
    friend struct table_impl_state_partial_split_somewhere;
    friend struct table_impl_state_partial_split_at_end;
    friend struct table_impl_state_full_split_somewhere;
    friend struct table_impl_state_full_split_at_end;
protected:
    static table_impl_state* states[12];

    /** Capacity of table in record count.*/
    record_index_t _record_capacity;
    /** Number of field per record.*/
    field_index_t _field_count;

    /** Time origin. Time of begining of first record (rec at index 0).*/
    record_time_t _origin = 0;
    /** Record duration. 0 if not used.*/
    record_time_t _duration = 0;

    /** Index of the first slot (position=0). */
    record_index_t _first_index = record::invalid_index();
    /** Index of the first stored record, -1 if no record is stored. */
    record_index_t _min_index = record::invalid_index();
    /** Position of the first stored record. */
    record_index_t _min_position = record::invalid_index();
    /** Index of the last stored record. */
    record_index_t _max_index = record::invalid_index();
    /** Position of the last stored record. */
    record_index_t _max_position = record::invalid_index();

    /** Stored field descriptors. */
    std::vector<field_impl> _fields;

    /** Concurrent access protection mutex. */
    mutable std::recursive_mutex _mutex;
    /** Alias for mutex guard. */
    typedef std::lock_guard<std::recursive_mutex> lock_t;

public:
    /** Default constructor. */
    base_table_impl() = default;
    /** Destructor. */
    virtual ~base_table_impl();

    /**
     * Create a table with specified fields and record capacity.
     * Shall throw an exception if creation fails.
     * @param fields Field descriptors used to create the table.
     * @param record_capacity Number of record the table shall be able to store.
     * @param origin Table time origin.
     * @param duration Table time duration.
     * @throw std::invalid_argument Fields list is empty.
     * This is a non-sense to create a table without fields.
     * @throw std::invalid_argument Record capacity of 0.
     * This is a non-sense to create a table without storage capacity.
     * @throw std::invalid_argument Invalid record capacity.
     */
    virtual void create(const std::vector<field_st>& fields, record_index_t record_capacity, record_time_t origin =0, record_time_t duration =0);

    field_index_t field_count() const override;
    const field_impl& field(field_index_t field)const override;
    const cyclic::field& field(const std::string& field_name)const override;

    record_index_t record_capacity() const override;
    record_index_t record_count() const override;
    record_index_t min_index()const override;
    record_index_t max_index()const override;

    record_time_t record_origin()const override;
    record_time_t record_duration()const override;
    record_index_t record_index(record_time_t time)const override;
    record_time_t record_time(record_index_t index)const override;

    std::unique_ptr<mutable_record> get_record() const override;
    std::unique_ptr<record> get_record(record_index_t index) const override;
    std::unique_ptr<record> get_record(record_time_t time) const override;

    void set_record(const record& rec) override;
    void set_record(record_index_t index, const record& rec) override;
    void set_record(record_time_t time, const record& rec) override;

    void update_record(const record& rec) override;
    void update_record(record_index_t index, const record& rec) override;
    void update_record(record_time_t time, const record& rec) override;

    void append_record() override;
    void append_record(record_index_t index) override;
    void append_record(record_time_t time) override;
    void append_record(const record& rec) override;
    void append_record(record_index_t index, const record& rec) override;
    void append_record(record_time_t time, const record& rec) override;

    void insert_record(record_index_t index) override;
    void insert_record(record_time_t time) override;
    void insert_record(const record& rec) override;
    void insert_record(record_index_t index, const record& rec) override;
    void insert_record(record_time_t time, const record& rec) override;

    virtual const_recordset_iterator begin()const override;
    virtual const_recordset_iterator end()const override;
protected:
    /**
     * Compute the position of a record from its index.
     * @param index Index of record.
     * @return Corresponding position, invalid_index() if out of range.
     * @note TODO Should it throw an exception if out of range ?
     */
    record_index_t index_to_position(record_index_t index)const;
    /**
     * Compute the index of a record from its position.
     * @param pos Position of the record.
     * @return Corresponding index, invalid_index() if out of range.
     * @note TODO Should it throw an exception if out of range ?
     */
    record_index_t position_to_index(record_index_t pos)const;

    /**
     * Retrieve the ID of the current state of the table.
     * @return Table current state ID.
     */
    TABLE_STATE get_internal_state_id()const;
    /**
     * Retrieve the implementation state table manipulator for current state.
     * @return Current implementation state table manipulator.
     */
    const table_impl_state* get_internal_state()const;

    /**
     * Retrieve the record stored at specified position.
     * Internal implementation method.
     * Shall be overriden by real storage implementations.
     * @param pos Position to look for.
     * @return Record having values at specified position.
     * @throw std::range_error Bad position parameter.
     */
    virtual raw_record get_record_at_position(record_index_t pos) const = 0;
    /**
     * Reset the record stored at specified position.
     * Internal implementation method.
     * Shall be overriden by real storage implementations.
     * @param pos Position to look for.
     * @throw std::range_error Bad position parameter.
     */
    virtual void reset_record_at_position(record_index_t pos) = 0;
    /**
     * Set the content of record stored at specified position.
     * Internal implementation method.
     * Shall be overriden by real storage implementations.
     * @param pos Position to look for.
     * @param rec Record to store.
     * @throw std::range_error Bad position parameter.
     */
    virtual void set_record_at_position(record_index_t pos, const record& rec) = 0;
    /**
     * Update the content of record stored at specified position.
     * Internal implementation method.
     * Default implementation, could be overriden by real storage implementations.
     * @param pos Position to look for.
     * @param rec Record with which update the record stored at specified position.
     * @throw std::range_error Bad position parameter.
     */
    virtual void update_record_at_position(record_index_t pos, const record& rec);

    /**
     * Implementation method used to flush table index descriptors to storage layer.
     * Do nothing by default, should be overriden by real implementation if needed.
     */
    virtual void write_table_index_descriptor();

    /**
     * Implementation of record iterator.
     */
    class iterator : public cyclic::recordset::const_iterator_interface
    {
    public:
        iterator() = default;
        iterator(const base_table_impl* table, record_index_t index = record::invalid_index());
        virtual ~iterator() = default;
        bool ok()const override;
        void increment() override;
        bool equals(const cyclic::recordset::const_iterator_interface* other)const override;
        const record& dereference() override;

    protected:
        std::shared_ptr<record> _rec;
        const base_table_impl* _table = nullptr;
        record_index_t _index = record::invalid_index();
    };
};

}}} // namespace cyclic::store::impl
#endif // _CYCLIC_LIBSTORE_BASE_IMPL_HPP_

