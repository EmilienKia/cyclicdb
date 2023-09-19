/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common-base.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/libcycliccommon is free software: you can redistribute it and/or
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

#include "common-base.hpp"

namespace cyclic
{


//
// raw_record
//

raw_record::raw_record(const cyclic::recordset* recordset, record_index_t record, record_time_t time) :
_recordset(recordset),
_index(record),
_time(time),
_values(recordset->field_count())
{
}

raw_record::raw_record(const raw_record& rec):
_recordset(rec._recordset),
_index(rec._index),
_time(rec._time),
_values(rec._values)
{
}

raw_record::raw_record(const record& rec) :
_recordset(rec.recordset()),
_index(rec.index()),
_time(rec.time())
{
    _values.resize(rec.size());
    for(field_index_t f = 0; f < rec.size(); ++f)
    {
        set(f, rec.get(f));
    }
}

raw_record::raw_record(raw_record&& rec) :
_recordset(rec._recordset),
_index(rec._index),
_time(rec._time),
_values(std::move(rec._values))
{
}

raw_record& raw_record::operator=(const record& rec)
{
    const raw_record* raw = dynamic_cast<const raw_record*>(&rec);
    if(raw!=nullptr)
    {
        _recordset = raw->_recordset;
        _index = raw->_index;
        _time = raw->_time;
        _values = raw->_values;
    }
    else
    {
        _recordset = rec.recordset();
        _index = rec.index();
        _time = rec.time();
        _values.resize(rec.size());
        for(field_index_t f = 0; f < rec.size(); ++f)
        {
            set(f, rec.get(f));
        }
    }
    return *this;
}

raw_record& raw_record::operator=(const raw_record& raw)
{
    _recordset = raw._recordset;
    _index = raw._index;
    _time = raw._time;
    _values = raw._values;
    return *this;
}

raw_record& raw_record::operator=(raw_record&& rec)
{
    _recordset = rec._recordset;
    _index = rec._index;
    _time = rec._time;
    _values = std::move(rec._values);
    return *this;
}

raw_record raw_record::raw(std::initializer_list<cyclic::value_t> init)
{
    raw_record rec;
    rec.fill(init);
    return rec;
}

const cyclic::recordset* raw_record::recordset()const
{
    return _recordset;
}

record_index_t raw_record::index()const
{
    return _index;
}

record_time_t raw_record::time()const
{
    return _time;
}

bool raw_record::ok()const
{
    return _index != invalid_index();
}

record& raw_record::attach(const cyclic::recordset* recordset)
{
    _recordset = recordset;
    // TODO Must I do some additional checks ?
    return *this;
}

record& raw_record::index(index_t index)
{
    _index = index;
    // TODO Must I do some additional checks ?
    return *this;
}

mutable_record& raw_record::time(time_t time)
{
    _time = time;
    return *this;
}

field_index_t raw_record::size()const
{
    return _values.size();
}

const field& raw_record::field(field_index_t field)const
{
    if(_recordset==nullptr)
    {
        throw cyclic::detached_record{};
    }
    else
    {
        return _recordset->field(field);
    }
}

const cyclic::field& raw_record::field(const std::string& field_name)const
{
    if(_recordset==nullptr)
    {
        throw cyclic::detached_record{};
    }
    else
    {
        return _recordset->field(field_name);
    }
}

bool raw_record::has(field_index_t field)const
{
    if(field >= _values.size())
    {
        throw std::out_of_range{""};
    }
    else
    {
        return _values[field].has_value();
    }
}

bool raw_record::has(const std::string& field_name)const
{
    field_index_t index = field(field_name).index();
    if(index>=_values.size())
    {
        throw cyclic::unknown_field{};
    }
    else
    {
        return _values[index].has_value();
    }
}

mutable_record& raw_record::reset(field_index_t field)
{
    ensure(field+1);
    _values[field].reset();
    return *this;
}

mutable_record& raw_record::reset(const std::string& field_name)
{
    field_index_t index = field(field_name).index();
    reset(index);
    return *this;
}

void raw_record::reset()
{
    for(field_index_t f = 0; f < size(); ++f)
    {
        reset(f);
    }
}

void raw_record::update(const record& rec)
{
    const raw_record* raw = dynamic_cast<const raw_record*>(&rec);
    if(raw != nullptr)
    {
        // Source record is a raw_record, use _values directly.
        ensure(_values.size());
        for(field_index_t f = 0; f < raw->_values.size(); ++f)
        {
            const value_t& val = raw->_values[f];
            if(val.has_value())
            {
                set(f, val);
            }
        }
    }
    else
    {
        // Source is a generic record, use generic updating algo.
        ensure(rec.size());
        for(field_index_t f = 0; f < rec.size(); ++f)
        {
            if(rec.has(f))
            {
                set(f, rec.get(f));
            }
        }
    }
}

const value_t& raw_record::get(field_index_t field)const
{
    if(field >= _values.size())
    {
        throw std::out_of_range{""};
    }
    else
    {
        return _values[field];
    }
}

const value_t& raw_record::get(const std::string& field_name)const
{
    field_index_t index = field(field_name).index();
    if(index>=_values.size())
    {
        throw cyclic::unknown_field{};
    }
    else
    {
        return _values[index];
    }
}

value_t& raw_record::get(field_index_t field)
{
    ensure(field+1);
    return _values[field];
}

value_t& raw_record::get(const std::string& field_name)
{
    field_index_t index = field(field_name).index();
    if(index>=_values.size())
    {
        throw cyclic::unknown_field{};
    }
    else
    {
        ensure(index+1);
        return _values[index];
    }
}

mutable_record& raw_record::set(field_index_t field, value_t value)
{
    ensure(field+1);
    _values[field] = value;
    return *this;
}

mutable_record& raw_record::set(const std::string& field_name, value_t value)
{
    field_index_t index = field(field_name).index();
    if(index>=_values.size())
    {
        throw cyclic::unknown_field{};
    }
    else
    {
        ensure(index+1);
        _values[index] = value;
    }
    return *this;
}

void raw_record::ensure(field_index_t field_count)
{
    if(field_count > _values.size())
    {
        _values.resize(field_count);
    }
}

void raw_record::fill(std::initializer_list<cyclic::value_t> init)
{
    _values.clear();
    _values.assign(init);
    // TODO Add more integrity check ?
}

} // namespace cyclic