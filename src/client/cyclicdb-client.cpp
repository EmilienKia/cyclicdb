/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/client/cyclicdb-client.cpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
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

#include "cyclicdb-client.hpp"

#include "cyclicdb.pb.h"
#include <google/protobuf/util/json_util.h>
#include "../common/cyclicdb-proto-serial.hpp"

#include "../store/libstore-mem-impl.hpp"

using namespace cyclic::proto;

namespace cyclic::client {

namespace internal {

struct field_descriptor : public cyclic::field {
    index_t _index;
    std::string _name;
    cyclic::data_type _type;

    field_descriptor() = default;

    field_descriptor(index_t index, const std::string &name, data_type type) :
        _index(index), _name(name),_type(type) {}

    index_t index() const override { return _index; }
    std::string name() const override { return _name; }
    data_type type() const override { return _type; }
};

struct ts_descriptor {
    std::vector<field_descriptor> fields;
    cyclic::record_index_t record_capacity;
    cyclic::record_time_t record_origin;
    cyclic::record_time_t record_duration;
    cyclic::record_index_t record_count;
    cyclic::record_index_t min_index;
    cyclic::record_index_t max_index;
};

}

//
// Timeseries
//
time_series::time_series(std::weak_ptr<connection> connection, const std::string& database, const std::string& name) :
        _connection(connection),
        _database(database),
        _name(name)
{
}

std::string time_series::name() const {
    return _name;
}

void time_series::refresh() {
    _connection.lock()->client()->get_timeseries_details(_database, _name);

    auto response = _connection.lock()->client()->get_timeseries_details(_database, _name);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
    } else {
        const timeseries_details& val = value(response);
        
        std::unique_ptr<internal::ts_descriptor> desc{new internal::ts_descriptor};
        desc->record_capacity = val.record_capacity;
        desc->record_origin = val.record_origin;
        desc->record_duration = val.record_duration;
        desc->record_count = val.record_count;
        // TODO : make min_index and max_index optionals
        desc->min_index = val.min_index ? *val.min_index : 0;
        desc->max_index = val.max_index ? *val.max_index : 0;
        for(size_t idx = 0; idx < val.fields.size(); ++idx) {
            auto& field = val.fields[idx];
            desc->fields.emplace_back(idx, field.name, field.type);
        }
        std::sort(desc->fields.begin(), desc->fields.end(),
                    [](const internal::field_descriptor& left, const internal::field_descriptor& right){ return left._index < right._index;});
        _desc = std::move(desc);
    }
}

void time_series::refresh_if_empty() const {
    if(!_desc) {
        const_cast<time_series*>(this)->refresh();
    }
}

field_index_t time_series::field_count() const {
    refresh_if_empty();
    return _desc->fields.size();
}

const field &time_series::field(field_index_t field) const {
    refresh_if_empty();
    return *std::find_if(_desc->fields.begin(), _desc->fields.end(), [=](const auto& f){return f._index == field;});
}

const field &time_series::field(const std::string &field_name) const {
    refresh_if_empty();
    return *std::find_if(_desc->fields.begin(), _desc->fields.end(), [=](const auto& f){return f._name == field_name;});
}

record_index_t time_series::record_count() const {
    refresh_if_empty();
    return _desc->record_count;
}

record_index_t time_series::min_index() const {
    refresh_if_empty();
    return _desc->min_index;
}

record_index_t time_series::max_index() const {
    refresh_if_empty();
    return _desc->max_index;
}

record_index_t time_series::record_capacity() const {
    refresh_if_empty();
    return _desc->record_capacity;
}

record_time_t time_series::record_origin() const {
    refresh_if_empty();
    return _desc->record_origin;
}

record_time_t time_series::record_duration() const {
    refresh_if_empty();
    return _desc->record_duration;
}

std::unique_ptr<mutable_record> time_series::get_record() const {
    return std::unique_ptr<mutable_record>(new raw_record(this));
}

record_index_t time_series::record_index(record_time_t time) const {
    refresh_if_empty();


    if(_desc->record_duration == 0)
    {
        // Time points are not supported.
        throw cyclic::time_not_supported{};
    }
    if(time < _desc->record_origin)
    {
        // Time point before begining of the table
        throw std::out_of_range{"Time before table origin."};
        // TODO support negative time duration
    }
    record_time_t index = (time - _desc->record_origin) / _desc->record_duration;
    if(index > record::absolute_max_index())
    {
        // Time out of valid index range.
        throw std::out_of_range{"Time out of valid index range."};
    }
    return index;
}

record_time_t time_series::record_time(record_index_t index) const {
    refresh_if_empty();


    if(_desc->record_duration == 0)
    {
        // Time points are not supported.
        throw cyclic::time_not_supported{};
    }
    return _desc->record_origin + index * _desc->record_duration;
}


std::unique_ptr<record> time_series::get_record(record_index_t index) const {
    auto response = _connection.lock()->client()->get_timeseries_data(_database, _name, index, index);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return nullptr;
    } else {
        const timeseries_data& val = value(response);

        if(val.field_count==0) {
            return {};
        }

        std::map<uint32_t, std::string> fields;
        for(size_t idx = 0; idx < val.fields.size(); ++idx) {
            const auto& field = val.fields[idx];
            fields.insert({idx, field.name});
        }

        auto rec = val.records.front();

        auto res = get_record();
        if(rec.record_index) {
            res->index(*rec.record_index);
        }
        // TODO Add time info
        for(auto val : rec.values) {
            res->set(fields[val.first], val.second);
        }
        return res;
    }
}

std::unique_ptr<record> time_series::get_record(record_time_t time) const {
    return get_record(record_index(time));
}

std::unique_ptr<recordset> time_series::get_recordset() const
{
    // TODO refacto this one
    return get_recordset(min_index(), max_index(), std::numeric_limits<record_index_t>::max());
}

std::unique_ptr<recordset> time_series::get_recordset(record_index_t first, record_index_t last, record_index_t count) const 
{
    // TODO take count in account
    // TODO refacto this one
    auto response = _connection.lock()->client()->get_timeseries_data(_database, _name, first, last);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return nullptr;
    } else {
        const timeseries_data& val = value(response);

        std::vector<raw_record> records;

        for(const auto& rec : val.records) {
            records.emplace_back(rec.record_index, rec.record_time, rec.values);
        }

        // TODO add origin and duration
        return std::make_unique<cyclic::store::impl::memory_recordset_impl>(val.fields, first, 0, 0, std::move(records));
    }
}

void time_series::set_record(const record &rec) {
    set_record(rec.index(), rec);
}

void time_series::set_record(record_index_t index, const record &rec) {
    // TODO: Not implemented yet, stub only.
}

void time_series::set_record(record_time_t time, const record &rec) {
    set_record(record_index(time), rec);
}

void time_series::update_record(const record &rec) {
    update_record(rec.index(), rec);
}

void time_series::update_record(record_index_t index, const record &rec) {
    // TODO: Not implemented yet, stub only.
}

void time_series::update_record(record_time_t time, const record &rec) {
    update_record(record_index(time), rec);
}

void time_series::append_record() {
    // TODO: Not implemented yet, stub only.
}

void time_series::append_record(record_index_t index) {
    // TODO: Not implemented yet, stub only.
}

void time_series::append_record(record_time_t time) {
    append_record(record_index(time));
}

void time_series::append_record(const record &rec) {
    append_record(rec.index(), rec);
}

void time_series::append_record(record_index_t index, const record &rec) {
    // TODO: Not implemented yet, stub only.
}

void time_series::append_record(record_time_t time, const record &rec) {
    append_record(record_index(time), rec);
}

void time_series::insert_record(record_index_t index) {
    // TODO: Not implemented yet, stub only.
}

void time_series::insert_record(record_time_t time) {
    insert_record(record_index(time));
}

void time_series::insert_record(const record &rec) {
    insert_record(rec.index(), rec);
}

void time_series::insert_record(record_index_t index, const record &rec) {
    timeseries_data_set_request req;

    // Fill field name-index mapping
    for(field_index_t idx = 0; idx < _desc->fields.size(); ++idx) {
        req.field_mapping.insert({idx, _desc->fields[idx].name()});
    }

    // Fill records
    timeseries_record_data data;
    data.record_index = rec.index();
    data.record_time = rec.time();
    for(field_index_t idx = 0; idx < _desc->fields.size(); ++idx) {
        if(rec.has(idx)) {
            data.values.insert({idx, rec.get(idx)});
        }
    }
    req.records.push_back(data);

    auto response = _connection.lock()->client()->set_timeseries_data(_database, _name, req);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
    } else {
        // Do nothing for now
    }
}

void time_series::insert_record(record_time_t time, const record &rec) {
    // TODO: Not implemented yet, stub only.
}


//
// Database
//
database::database(std::weak_ptr<connection> connection, const std::string& name) :
    _connection(connection),
    _name(name)
{
}

std::string database::name() const {
    return _name;
}

std::vector<std::shared_ptr<time_series>> database::get_time_series() {
    auto response = _connection.lock()->client()->list_timeseries(_name);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return {};
    } else {
        const list_timeseries_response& val = value(response);

        std::vector<std::shared_ptr<time_series>> res;
        for(const auto& name : val.timeseries) {
            res.push_back(std::shared_ptr<time_series>{new time_series(_connection, _name, name)});
        }
        return res;
    }
}

std::shared_ptr<time_series> database::get_time_series(const std::string& name) {
    std::shared_ptr<time_series> ts{new time_series(_connection, _name, name)};
    ts->refresh();
    return ts;
}

std::shared_ptr<time_series> database::create_time_series(const std::string& name, const std::vector<cyclic::field_st>& columns, cyclic::record_index_t capacity)
{
    create_timeseries_request req;
    req.tsname = name;
    req.capacity = capacity;
    req.origin = 0;
    req.duration = 0;
    for(const auto& column : columns) {
        req.fields.push_back(column);
    }

    auto response = _connection.lock()->client()->create_timeseries(_name, req);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO throw exception
        return nullptr;
    } else {
        const create_timeseries_response& val = value(response);
        return std::shared_ptr<time_series>{new time_series(_connection, _name, val.tsname)};
    }
}


//
// Connection
//

connection::connection(const std::string& url):
_url(url),
_client(cyclic_http_client::create(url))
{
}

connection::~connection() {
}

std::shared_ptr<connection> connection::create_connection(const std::string& url) {
    return std::shared_ptr<connection>{new connection(url)};
}

size_t connection::get_database_count() {
    return get_databases().size();
}

std::vector<std::shared_ptr<database>> connection::get_databases() {
    auto response = _client->list_database_names();
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return {};
    } else {
        std::vector<std::shared_ptr<database>> res;
        for(const auto& name : value(response).databases) {
            res.push_back(std::shared_ptr<database>{new database(shared_from_this(), name)});
        }
        return res;
    }
}


std::shared_ptr<database> connection::create_database(const std::string& name) {
    auto response = _client->create_database(name);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return nullptr;
    } else {
        const create_database_response& val = value(response);
        return std::shared_ptr<database>{new database(std::weak_ptr(shared_from_this()), val.dbname)};
    }
}


std::shared_ptr<database> connection::get_database(const std::string& name) {
    auto response = _client->get_database(name);
    if(!cyclic::proto::succeed(response)) {
        const error_report& err = error(response);
        std::cerr << "Error: " << err << std::endl;
        // TODO thrown exception
        return nullptr;
    } else {
        const get_database_response& val = value(response);
        return std::shared_ptr<database>{new database(std::weak_ptr(shared_from_this()), val.dbname)};
    }
}

} // namespace cyclic::client
