/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store/libstore-mem-impl.cpp
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

#include "libstore-mem-impl.hpp"

#include <iostream>
#include <sstream>

/*
A table can be in many states (12):

 0   1   2   3   4   5   6   7   8   9   10  11

miax
                                                          +-
    miax         min min         #   #   #   #        0   |
                 #   #           #   #   #   #            +
                 #   #           max max max #        1   |
                 #   #                       #            +
                 #   #   min min         min #        2   |
                 #   #   #   #           #   #            +
        miax     max #   #   #   min     #   #        3   |
                     #   #   #   #       #   #            +
                     #   max #   #       #   #        4   |
                     #       #   #       #   #            +
                     #       #   #       #   #        ... |
                     #       #   #       #   #            +
                     #       #   #       #   max      n-1 |
                     #       #   #       #                +
            miax     max     max #   min #   min      n   |
                                                          +--

(with positions: n last possible position and xand y some intermediate positions with x<y)

No record:
 0 - there is no record
Only one record:
 1 - min=max at pos 0
 2 - min=max at pos x
 3 - min=max at pos n
Contiguous records:
 4 - min<max with min=0 and max=x
 5 - min<max with min=0 and max=n
 6 - min<max with min=x and max=y
 7 - min<max with min=x and max=n
Split records (after a loop):
 8 - min>max with min=x and max=y
 9 - min>max with min=n and max=x
10 - min>max with min=y and max=x and min=max+1
11 - min>max with min=n and max=n-1 and min=max+1
 */


namespace cyclic::store::impl
{

//
// memory_recordset_impl
//
memory_recordset_impl::memory_recordset_impl(const std::vector<field_st>& fields, record_index_t first_index, record_time_t origin , record_time_t duration, const std::vector<raw_record>& data):
_data(data)
{
    size_t index = 0;
    for(const field_st& f : fields)
    {
        _fields.emplace_back(f.type, index++, f.name);
    }
    _first_index = first_index;
    _origin = origin;
    _duration = duration;
}

memory_recordset_impl::memory_recordset_impl(const std::vector<field_st>& fields, record_index_t first_index, record_time_t origin , record_time_t duration, std::vector<raw_record>&& data):
_data(data)
{
    size_t index = 0;
    for(const field_st& f : fields)
    {
        _fields.emplace_back(f.type, index++, f.name);
    }
    _first_index = first_index;
    _origin = origin;
    _duration = duration;
}

record_index_t memory_recordset_impl::record_count() const
{
    return _data.size();
}

std::unique_ptr<record> memory_recordset_impl::get_record(record_index_t index) const 
{
    if(index < _first_index+_data.size())
    {
        return std::make_unique<raw_record>(_data[index-_first_index]);
    }
    else
    {
        throw std::range_error{"Internal getting record position error"};
    }
}

//
// memory_table_impl
//

void memory_table_impl::create(const std::vector<field_st>& fields, record_index_t record_capacity,
        record_time_t origin, record_time_t duration)
{
    base_table_impl::create(fields, record_capacity, origin, duration);
    _data.clear();
    _data.reserve(record_capacity);
    _data.resize(record_capacity, raw_record{this});
}

raw_record memory_table_impl::get_record_at_position(record_index_t pos) const
{
    if(pos < _record_capacity)
    {
        return _data[pos];
    }
    else
    {
        throw std::range_error{"Internal getting record position error"};
    }
}

void memory_table_impl::reset_record_at_position(record_index_t pos)
{
    if(pos < _record_capacity)
    {
        _data[pos] = raw_record{this};
    }
    else
    {
        throw std::range_error{"Internal reseting record position error"};
    }
}

void memory_table_impl::set_record_at_position(record_index_t pos, const record& rec)
{
    if(pos < _record_capacity)
    {
        _data[pos] = rec;
    }
    else
    {
        throw std::range_error{"Internal setting record position error"};
    }

}

std::unique_ptr<recordset> memory_table_impl::get_recordset(record_index_t first, record_index_t last, record_index_t count) const
{
    std::vector<field_st> fields;
    for(const auto& field : _fields) {
        fields.push_back(field_st{.name=field.name(), .type=field.type()});
    }

    std::vector<raw_record> data;
    data.reserve(count);

    for(record_index_t i = first; i < last && count-- > 0; ++i)
    {
        data.push_back( _data[ index_to_position(i) ] );
    }

    return std::make_unique<memory_recordset_impl>(fields, first, _origin, _duration, std::move(data));
}

} // namespace cyclic::store::impl