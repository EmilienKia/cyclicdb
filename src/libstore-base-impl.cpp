/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore-base-impl.cpp
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

#include "libstore-base-impl.hpp"

#include <iostream>
#include <sstream>

namespace cyclic
{
namespace store
{
namespace impl
{

//
// field_impl
//

field_impl::field_impl()
{
}

field_impl::field_impl(const field_impl& field) :
_type(field._type),
_index(field._index),
_name(field._name),
_size(field._size),
_offset(field._offset)
{
}

field_impl::field_impl(field_impl&& field) :
_type(field._type),
_index(field._index),
_name(std::move(field._name)),
_size(field._size),
_offset(field._offset)
{
}

field_impl::field_impl(data_type type, field_index_t index, const std::string& name, uint16_t size, uint16_t offset) :
_type(type),
_index(index),
_name(name),
_size(size),
_offset(offset)
{
}

field_impl& field_impl::operator=(const field_impl& field)
{
    this->_type = field._type;
    this->_index = field._index;
    this->_name = field._name;
    this->_size = field._size;
    this->_offset = field._offset;
    return *this;
}

field_impl& field_impl::operator=(field_impl&& field)
{
    this->_type = field._type;
    this->_index = field._index;
    this->_name = std::move(field._name);
    this->_size = field._size;
    this->_offset = field._offset;
    return *this;
}

field_index_t field_impl::index() const
{
    return _index;
}

std::string field_impl::name() const
{
    return _name;
}

data_type field_impl::type() const
{
    return _type;
}

uint16_t field_impl::size() const
{
    return _size;
}

uint16_t field_impl::offset() const
{
    return _offset;
}

//
// table_impl_state
//

struct table_impl_state
{
    virtual ~table_impl_state() = default;
    virtual TABLE_STATE state() const = 0;
    virtual record_index_t do_append_record(base_table_impl& table) const = 0;
};

struct table_impl_state_no_record /* 0 */ : public table_impl_state
{
    virtual ~table_impl_state_no_record() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_NO_RECORD;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._min_index = 0;
        table._max_index = 0;
        table._min_position = 0;
        table._max_position = 0;
        return 0;
    }
};

struct table_impl_state_one_record_at_begining /* 1 */ : public table_impl_state
{
    virtual ~table_impl_state_one_record_at_begining() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_ONE_RECORD_AT_BEGINING;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_one_record_somewhere /* 2 */ : public table_impl_state
{
    virtual ~table_impl_state_one_record_somewhere() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_ONE_RECORD_SOMEWHERE;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_one_record_at_end /* 3 */ : public table_impl_state
{
    virtual ~table_impl_state_one_record_at_end() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_ONE_RECORD_AT_END;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position = 0;
        return table._max_index;
    }
};

struct table_impl_state_partial_contiguous_at_begining /* 4 */ : public table_impl_state
{
    virtual ~table_impl_state_partial_contiguous_at_begining() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_PARTIAL_CONTIGUOUS_AT_BEGINING;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_full_contiguous /* 5 */ : public table_impl_state
{
    virtual ~table_impl_state_full_contiguous() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_FULL_CONTIGUOUS;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._min_index++;
        table._min_position++;
        table._max_index++;
        table._max_position = 0;
        table._first_index = table._max_index;
        return table._max_index;
    }
};

struct table_impl_state_partial_contiguous_somewhere /* 6 */ : public table_impl_state
{
    virtual ~table_impl_state_partial_contiguous_somewhere() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_PARTIAL_CONTIGUOUS_SOMEWHERE;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_partial_contiguous_at_end /* 7 */ : public table_impl_state
{
    virtual ~table_impl_state_partial_contiguous_at_end() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_PARTIAL_CONTIGUOUS_AT_END;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position = 0;
        table._first_index = table._max_index;
        return table._max_index;
    }
};

struct table_impl_state_partial_split_somewhere /* 8 */ : public table_impl_state
{
    virtual ~table_impl_state_partial_split_somewhere() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_PARTIAL_SPLIT_SOMEWHERE;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        //table.reset_record_at_position(table._max_position);
        return table._max_index;
    }
};

struct table_impl_state_partial_split_at_end /* 9 */ : public table_impl_state
{
    virtual ~table_impl_state_partial_split_at_end() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_PARTIAL_SPLIT_AT_END;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_full_split_somewhere /* 10 */ : public table_impl_state
{
    virtual ~table_impl_state_full_split_somewhere() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_FULL_SPLIT_SOMEWHERE;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._min_index++;
        table._min_position++;
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

struct table_impl_state_full_split_at_end /* 11 */ : public table_impl_state
{
    virtual ~table_impl_state_full_split_at_end() = default;

    virtual TABLE_STATE state() const
    {
        return TABLE_FULL_SPLIT_AT_END;
    }

    virtual record_index_t do_append_record(base_table_impl& table) const
    {
        table._min_index++;
        table._min_position = 0;
        table._max_index++;
        table._max_position++;
        return table._max_index;
    }
};

static table_impl_state_no_record table_impl_state_0;
static table_impl_state_one_record_at_begining table_impl_state_1;
static table_impl_state_one_record_somewhere table_impl_state_2;
static table_impl_state_one_record_at_end table_impl_state_3;
static table_impl_state_partial_contiguous_at_begining table_impl_state_4;
static table_impl_state_full_contiguous table_impl_state_5;
static table_impl_state_partial_contiguous_somewhere table_impl_state_6;
static table_impl_state_partial_contiguous_at_end table_impl_state_7;
static table_impl_state_partial_split_somewhere table_impl_state_8;
static table_impl_state_partial_split_at_end table_impl_state_9;
static table_impl_state_full_split_somewhere table_impl_state_10;
static table_impl_state_full_split_at_end table_impl_state_11;

//
// base_table_impl
//

table_impl_state* base_table_impl::states[12]{
    &table_impl_state_0,
    &table_impl_state_1,
    &table_impl_state_2,
    &table_impl_state_3,
    &table_impl_state_4,
    &table_impl_state_5,
    &table_impl_state_6,
    &table_impl_state_7,
    &table_impl_state_8,
    &table_impl_state_9,
    &table_impl_state_10,
    &table_impl_state_11
};

base_table_impl::~base_table_impl()
{
}

void base_table_impl::create(const std::vector<field_st>& fields, record_index_t record_capacity,
        record_time_t origin, record_time_t duration)
{
    if(fields.empty())
    {
        throw std::invalid_argument{"fields cannot be empty."};
    }
    if(record_capacity==0)
    {
        throw std::invalid_argument{"record_capacity cannot be 0."};
    }
    if(record_capacity==record::invalid_index())
    {
        throw std::invalid_argument{"record_capacity cannot be invalid."};
    }

    _field_count = fields.size();
    _record_capacity = record_capacity;

    _origin = origin;
    _duration = duration;

    size_t index = 0;
    for(const field_st& f : fields)
    {
        _fields.emplace_back(f.type, index++, f.name);
    }
}

field_index_t base_table_impl::field_count() const
{
    return _field_count;
}

const field_impl& base_table_impl::field(field_index_t field)const
{
    if(field < _fields.size())
    {
        return _fields[field];
    }
    else
    {
        std::ostringstream stm;
        stm << "Out of range field id (" << field << " / " << _fields.size() << ") .";
        throw std::out_of_range(stm.str());
    }
}

const cyclic::field& base_table_impl::field(const std::string& field_name)const
{
    for(const field_impl& field : _fields)
    {
        if(field.name() == field_name)
        {
            return field;
        }
    }
    throw unknown_field{};
}

record_index_t base_table_impl::record_capacity() const
{
    return _record_capacity;
}

record_index_t base_table_impl::record_count() const
{
    return _min_index == record::invalid_index() ? 0 : _max_index - _min_index + 1;
}

record_index_t base_table_impl::min_index()const
{
    return _min_index;
}

record_index_t base_table_impl::max_index()const
{
    return _max_index;
}

record_time_t base_table_impl::record_origin()const
{
    return _origin;
}

record_time_t base_table_impl::record_duration()const
{
    return _duration;
}

record_index_t base_table_impl::record_index(record_time_t time)const
{
    if(_duration == 0)
    {
        // Time points are not supported.
        throw cyclic::time_not_supported{};
    }
    if(time < _origin)
    {
        // Time point before begining of the table
        throw std::out_of_range{"Time before table origin."};
        // TODO support negative time duration
    }
    record_time_t index = (time - _origin) / _duration;
    if(index > record::absolute_max_index())
    {
        // Time out of valid index range.
        throw std::out_of_range{"Time out of valid index range."};
    }
    return index;
}

record_time_t base_table_impl::record_time(record_index_t index)const
{
    if(_duration == 0)
    {
        // Time points are not supported.
        throw cyclic::time_not_supported{};
    }
    return _origin + index * _duration;
}

record_index_t base_table_impl::index_to_position(record_index_t index)const
{
    lock_t lock{_mutex};
    if(index < _min_index || index > _max_index) return record::invalid_index(); // Out of range index
    if(index == _min_index) return _min_position;
    if(index == _max_index) return _max_position;
    if(index == _first_index) return 0;

    if(_min_position <= _max_position)
    {
        // Contiguous ranges
        return _min_position + index - _min_index;
    }
    else
    {
        // Split ranges
        if(index >= _first_index)
        {
            return index - _first_index;
        }
        else
        {
            return _min_position + index - _min_index;
        }
    }
}

record_index_t base_table_impl::position_to_index(record_index_t pos)const
{
    lock_t lock{_mutex};
    if(pos >= _record_capacity) return -2; // Position out of capacity
    if(_min_index == record::invalid_index() || _max_index == record::invalid_index()) return record::invalid_index(); // Not used position
    if(pos == _min_position) return _min_index;
    if(pos == _max_position) return _max_index;

    if(_min_position <= _max_position)
    {
        // Contiguous ranges
        if(pos > _min_position && pos < _max_position)
        {
            return _min_index + pos - _min_position;
        }
    }
    else
    {
        // Split ranges
        if(pos < _max_position)
        {
            return _first_index + pos;
        }
        else if(pos > _min_position)
        {
            return _min_index + pos - _min_position;
        }
    }
    return record::invalid_index(); // Not used position
}

TABLE_STATE base_table_impl::get_internal_state_id()const
{
    lock_t lock{_mutex};
    if(_min_index == record::invalid_index()) return TABLE_NO_RECORD; // No record

    else if(_min_index == _max_index) // Only one record
    {
        if(_min_position == 0) return TABLE_ONE_RECORD_AT_BEGINING;
        else if(_min_position == _record_capacity - 1) return TABLE_ONE_RECORD_AT_END;
        else return TABLE_ONE_RECORD_SOMEWHERE;
    }

    else if(_min_position < _max_position) // Contiguous records
    {
        if(_min_position == 0)
        {
            if(_max_position < _record_capacity - 1) return TABLE_PARTIAL_CONTIGUOUS_AT_BEGINING;
            else return TABLE_FULL_CONTIGUOUS;
        }
        else
        {
            if(_max_position < _record_capacity - 1) return TABLE_PARTIAL_CONTIGUOUS_SOMEWHERE;
            else return TABLE_PARTIAL_CONTIGUOUS_AT_END;
        }
    }

    else // Split records
    {
        if(_min_position > _max_position + 1) // Partial records
        {
            if(_min_position < _record_capacity - 1) return TABLE_PARTIAL_SPLIT_SOMEWHERE;
            else return TABLE_PARTIAL_SPLIT_AT_END;
        }
        else // Full records
        {
            if(_min_position < _record_capacity - 1) return TABLE_FULL_SPLIT_SOMEWHERE;
            else return TABLE_FULL_SPLIT_AT_END;
        }
    }
}

const table_impl_state* base_table_impl::get_internal_state()const
{
    return states[get_internal_state_id()];
}

std::unique_ptr<mutable_record> base_table_impl::get_record()const
{
    return std::unique_ptr<mutable_record>(new raw_record(this));
}

std::unique_ptr<record> base_table_impl::get_record(record_index_t index)const
{
    record_index_t pos = index_to_position(index);
    if(pos != record::invalid_index())
    {
        raw_record* rec = new raw_record{get_record_at_position(pos)};
        rec->index(index);
        if(_duration!=0)
        {
            rec->time(record_time(index));
        }
        rec->attach(this);
        return std::unique_ptr<record>{rec};
    }
    else
    {
        return std::unique_ptr<record>();
    }
}

std::unique_ptr<record> base_table_impl::get_record(record_time_t time)const
{
    return get_record(record_index(time));
}

void base_table_impl::set_record(const record& rec)
{
    set_record(rec.index(), rec);
}

void base_table_impl::set_record(record_index_t index, const record& rec)
{
    lock_t lock{_mutex};
    if(index == record::invalid_index())
    {
        // Bad parameter value
        throw std::invalid_argument{"Index cannot be invalid."};
    }

    if(_min_index == record::invalid_index())
    {
        // Empty table : cannot set a value to a record.
        throw std::logic_error{"Cannot set a record on an empty table."};
    }

    if(index < _min_index || index > _max_index)
    {
        // Index out of range.
        throw std::out_of_range{"Cannot set a record at table out-of-range index."};
    }

    record_index_t pos = index_to_position(index);
    if(pos != record::invalid_index())
    {

        set_record_at_position(pos, rec);
        write_table_index_descriptor(); // TODO Is really needed as we dont append new record ?
    }
    else
    {
        throw std::range_error{"Internal index computation error"};
    }
}

void base_table_impl::set_record(record_time_t time, const record& rec)
{
    set_record(record_index(time), rec);
}

void base_table_impl::update_record(const record& rec)
{
    update_record(rec.index(), rec);
}

void base_table_impl::update_record(record_index_t index, const record& rec)
{
    lock_t lock{_mutex};
    if(index == record::invalid_index())
    {
        // Bad parameter value
        throw std::invalid_argument{"Index cannot be invalid."};
    }

    if(_min_index == record::invalid_index())
    {
        // Empty table : cannot update a value to a record.
        throw std::logic_error{"Cannot update a record on an empty table."};
    }

    if(index < _min_index || index > _max_index)
    {
        // Index out of range.
        throw std::out_of_range{"Cannot update a record at table out-of-range index."};
    }

    record_index_t pos = index_to_position(index);
    if(pos != record::invalid_index())
    {
        update_record_at_position(pos, rec);
        write_table_index_descriptor(); // TODO Is really needed as we dont append new record ?
    }
    else
    {
        throw std::range_error{"Internal index computation error"};
    }
}

void base_table_impl::update_record(record_time_t time, const record& rec)
{
    update_record(record_index(time), rec);
}

void base_table_impl::append_record()
{
    get_internal_state()->do_append_record(*this);
    reset_record_at_position(_max_position);
    write_table_index_descriptor();
}

void base_table_impl::append_record(record_index_t index)
{
    lock_t lock{_mutex};
    if(index == record::invalid_index())
    {
        if(_min_index == record::invalid_index())
            index = 0; // Table is empty, append to first record index.
        else
            index = _max_index + 1; // Table is not empty, append just after the last index.
    }

    if(_min_index != record::invalid_index() && index <= _max_index)
    {
        // Bad parameter value : intend to append before end of existing records.
        // True only if table is not empty (otherwise _max_index is always upper than index).
        throw std::out_of_range{"Cannot append a record before end of table."};
    }

    do
    {
        // Insert records up to correct index
        get_internal_state()->do_append_record(*this);
        reset_record_at_position(_max_position);
    }    while(_max_index < index);
    // Note : do not test before inserting to be sure to insert a rec on empty tables.
    write_table_index_descriptor();
}

void base_table_impl::append_record(record_time_t time)
{
    append_record(record_index(time));
}

void base_table_impl::append_record(const record& rec)
{
    append_record(rec.index(), rec);
}

void base_table_impl::append_record(record_index_t index, const record& rec)
{
    lock_t lock{_mutex};
    // If the index is not set (invalid), append just after the last record
    if(index == record::invalid_index())
    {
        if(_min_index == record::invalid_index())
            index = 0;
        else
            index = _max_index + 1;
    }

    if(_min_index != record::invalid_index() && index <= _max_index)
    {
        // Table is not empty and intend to append before end of existing index.
        throw std::out_of_range{"Cannot append a record before end of table."};
    }
    else if(_min_index == record::invalid_index() && index == 0)
    {
        // Table is empty and inserting at first index
        get_internal_state()->do_append_record(*this);
        set_record_at_position(_max_position, rec);
    }
    else if(_max_index == record::absolute_max_index())
    {
        // Table is full : max capacity reached
        throw table_is_full{"Table is full, no more record can be append"};
    }
    else
    {
        // Append empty rec before target index
        while(_max_index < index - 1)
        {
            get_internal_state()->do_append_record(*this);
            reset_record_at_position(_max_position);
        }

        // Append record at target index
        //if(max_index() < index) // Shall be always true
        //{
        get_internal_state()->do_append_record(*this);
        set_record_at_position(_max_position, rec);
        //}

        write_table_index_descriptor();
    }
}

void base_table_impl::append_record(record_time_t time, const record& rec)
{
    append_record(record_index(time), rec);
}

void base_table_impl::insert_record(record_index_t index)
{
    lock_t lock{_mutex};
    if(index < _min_index)
    {
        // Index is before first record.
        throw std::out_of_range{"Cannot insert a record before begining of table."};
    }
    else if(index <= _max_index)
    {
        // Index is already used.
        // Do nothing.
    }
    else
    {
        // Index is after last record or not set (invalid)
        // Append a record
        append_record(index);
    }
}

void base_table_impl::insert_record(record_time_t time)
{
    insert_record(record_index(time));
}

void base_table_impl::insert_record(const record& rec)
{
    insert_record(rec.index(), rec);
}

void base_table_impl::insert_record(record_index_t index, const record& rec)
{
    lock_t lock{_mutex};
    if(index < _min_index)
    {
        // Index is before first record.
        throw std::out_of_range{"Cannot insert a record before begining of table."};
    }
    else if(index <= _max_index)
    {
        // Index is already used.
        // Set the record
        set_record(index, rec);
    }
    else
    {
        // Index is after last record or not set (invalid)
        // Append a record
        append_record(index, rec);
    }
}

void base_table_impl::insert_record(record_time_t time, const record& rec)
{
    insert_record(record_index(time), rec);
}

void base_table_impl::update_record_at_position(record_index_t pos, const record& rec)
{
    raw_record curr = get_record_at_position(pos);
    curr.update(rec);
    set_record_at_position(pos, curr);
}

void base_table_impl::write_table_index_descriptor()
{
    // Do nothing by default
}

const_recordset_iterator base_table_impl::begin()const
{
    return const_recordset_iterator{std::make_shared<iterator>(this, min_index())};
}

const_recordset_iterator base_table_impl::end()const
{
    return const_recordset_iterator{std::make_shared<iterator>(this)};
}

//
// base_table_impl::iterator
//
base_table_impl::iterator::iterator(const base_table_impl* table, record_index_t index):
_table(table),
_index(index)
{
}

bool base_table_impl::iterator::ok()const
{
    if(!_table)
    {
        return false;
    }
    if(_table->record_count()==0)
    {
        return false;
    }
    return _index <= _table->max_index();
}

void base_table_impl::iterator::increment()
{
    if(_table && _index!=record::invalid_index())
    {
        _index++;
        _rec.reset();
    }
}

bool base_table_impl::iterator::equals(const cyclic::recordset::const_iterator_interface* other)const
{
    const base_table_impl::iterator* it = dynamic_cast<const base_table_impl::iterator*>(other);
    if(it==nullptr)
    {
        return false;
    }
    if(_table!=it->_table)
    {
        return false;
    }
    if(_index<=_table->max_index())
    {
        return _index == it->_index;
    }
    else
    {
        return it->_index > it->_table->max_index();
    }
}

const record& base_table_impl::iterator::dereference()
{
    if(!_rec)
    {
        if(_table!=nullptr)
        {
            _rec = _table->get_record(_index);
        }
    }
    return *_rec;
}


}
}
} // namespace cyclic::store::impl