/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-service.hpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/libcyclicdbsrv is free software: you can redistribute it and/or
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

#include "cyclicdb-service.hpp"

#include "libstore.hpp"

#include <iostream>

namespace cyclic::server {

namespace fs = std::filesystem;

static constexpr auto MAIN_TS_CYDB_FILENAME = "ts.cydb";

//
// Time series
//

time_series::time_series(std::unique_ptr<cyclic::table> table) :
_table(std::move(table))
{
}

std::shared_ptr<time_series> time_series::load_time_series(const std::filesystem::path& path) {
    // Check the path exists and is a valid ts directory
    if(!fs::exists(path) || !fs::is_directory(path)) {
        // Error: path must exist and be a directory
        // TODO throw an exception
    }

    auto ts = std::shared_ptr<time_series>{new time_series(
            cyclic::store::file::open(path / MAIN_TS_CYDB_FILENAME)
    )};
    return ts;
}


field_index_t time_series::field_count() const {
    return _table->field_count();
}

const field &time_series::field(field_index_t field) const {
    return _table->field(field);
}

const field &time_series::field(const std::string &field_name) const {
    return _table->field(field_name);
}

record_index_t time_series::record_count() const {
    return _table->record_count();
}

record_index_t time_series::min_index() const {
    return _table->min_index();
}

record_index_t time_series::max_index() const {
    return _table->max_index();
}

const_recordset_iterator time_series::begin() const {
    return _table->begin();
}

const_recordset_iterator time_series::end() const {
    return _table->end();
}

record_index_t time_series::record_capacity() const {
    return _table->record_capacity();
}

record_time_t time_series::record_origin() const {
    return _table->record_origin();
}

record_time_t time_series::record_duration() const {
    return _table->record_duration();
}

record_index_t time_series::record_index(record_time_t time) const {
    return _table->record_index(time);
}

record_time_t time_series::record_time(record_index_t index) const {
    return _table->record_time(index);
}

std::unique_ptr<mutable_record> time_series::get_record() const {
    return _table->get_record();
}

std::unique_ptr<record> time_series::get_record(record_index_t index) const {
    return _table->get_record(index);
}

std::unique_ptr<record> time_series::get_record(record_time_t time) const {
    return _table->get_record(time);
}

void time_series::set_record(const record &rec) {
    _table->set_record(rec);
}

void time_series::set_record(record_index_t index, const record &rec) {
    _table->set_record(index, rec);
}

void time_series::set_record(record_time_t time, const record &rec) {
    _table->set_record(time, rec);
}

void time_series::update_record(const record &rec) {
    _table->update_record(rec);
}

void time_series::update_record(record_index_t index, const record &rec) {
    _table->update_record(index, rec);
}

void time_series::update_record(record_time_t time, const record &rec) {
    _table->update_record(time, rec);
}

void time_series::append_record() {
    _table->append_record();
}

void time_series::append_record(record_index_t index) {
    _table->append_record(index);
}

void time_series::append_record(record_time_t time) {
    _table->append_record(time);
}

void time_series::append_record(const record &rec) {
    _table->append_record(rec);
}

void time_series::append_record(record_index_t index, const record &rec) {
    _table->append_record(index, rec);
}

void time_series::append_record(record_time_t time, const record &rec) {
    _table->append_record(time, rec);
}

void time_series::insert_record(record_index_t index) {
    _table->insert_record(index);
}

void time_series::insert_record(record_time_t time) {
    _table->insert_record(time);
}

void time_series::insert_record(const record &rec) {
    _table->insert_record(rec);
}

void time_series::insert_record(record_index_t index, const record &rec) {
    _table->insert_record(index, rec);
}

void time_series::insert_record(record_time_t time, const record &rec) {
    _table->insert_record(time, rec);
}


//
// Database
//

database::database(const std::string &name, const fs::path &path) :
_name(name), _path(path)
{
}

std::shared_ptr<database> database::load_database(const std::string& name, const std::filesystem::path& path) {
    // Ensure the directory exists
    if(!fs::exists(path) && !fs::is_directory(path)) {
        // Error: the database directory doesnt exists or is not a directory
        // TODO throw an exception
    }

    std::shared_ptr<database> db{new database(name, path)};

    // TODO Check it is database directory
    // TODO Load configuration if exists

    // List database directory time series content
    for(auto dir_entry : fs::directory_iterator(path, ~fs::directory_options::none)) {
        if(dir_entry.exists() && dir_entry.is_directory()) {
            db->load_time_series(dir_entry.path());
        }
    }

    return db;
}

std::shared_ptr<time_series> database::load_time_series(const std::filesystem::path& path) {
    // TODO check the time series is not already loaded
    auto ts = time_series::load_time_series(path);
    if(ts) {
        std::string name = path.filename();
        _series.insert({name, ts});
    }
    return ts;
}


size_t database::get_time_series_count() const {
    return _series.size();
}

std::vector<std::string> database::list_time_series_names() const {
    std::vector<std::string> res;
    for(const auto & entry : _series) {
        res.push_back(entry.first);
    }
    return res;
}

std::shared_ptr<time_series> database::get_time_series(const std::string& name) const {
    if(auto it = _series.find(name); it != _series.end()) {
        return it->second;
    } else {
        return {};
    }
}


std::shared_ptr<time_series> database::create_time_series(const std::string& ts_name,
                                                const std::vector<field_st>& fields,
                                                record_index_t record_capacity,
                                                record_time_t origin, record_time_t duration) {
    // TODO check ts name, must be file compatible

    // Check ts name is not already existing
    if(_series.find(ts_name) != _series.end()) {
        // Error: cannot create a new ts with an existing name
        // TODO throw an exception
    }

    // Create the TS directory
    if(!fs::create_directories(_path / ts_name)) {
        // Error:  cannot create the ts directory
        // TODO throw an exception
    }

    auto ts = std::shared_ptr<time_series>{new time_series(
            cyclic::store::file::create(_path / ts_name / MAIN_TS_CYDB_FILENAME, cyclic::store::file::COMPACT, fields, record_capacity, origin, duration)
            )};
    _series.insert({ts_name, ts});

    return ts;
}



//
// Service
//


service::service(const std::string &path) :
_path(path)
{
    // TODO Load database stubs at service creation
}

size_t service::get_database_count() const {
    return _databases.size();
}

std::shared_ptr<database> service::create_database(const std::string &name) {
    // TODO Check if the db to be created is not in conflict
    // TODO Check the name to be fs compatible

    auto db_path = _path / name;
    if(!fs::create_directory(db_path)) {
        // Error: cannot create the database directory
        // TODO throw an exception
    }

    // TODO Check the database is not already loaded and not in name collision
    std::shared_ptr<database> db{new database(name, db_path)};
    _databases.insert({name, db});
    return db;
}

std::shared_ptr<database> service::get_database(const std::string &name) const{
    auto it = _databases.find(name);
    if(it!=_databases.end()) {
        return it->second;
    } else {
        return {};
    }
}

std::vector<std::string> service::list_database_names() const {
    std::vector<std::string> res;
    for(const auto & entry : _databases) {
        res.push_back(entry.first);
    }
    return res;
}

std::shared_ptr<service> service::create_service(const fs::path &path) {
    // Ensure the directory exists
    if(!fs::create_directory(path)) {
        // Error: the service directory cannot be created
        // TODO throw an exception
    }
    // TODO Check the directory is empty - or at least it is not already holding an existing service
    return std::shared_ptr<service>(new service(path));
}

std::shared_ptr<service> service::load_service(const fs::path &path) {
    // Ensure the directory exists
    if(!fs::exists(path) && !fs::is_directory(path)) {
        // Error: the service directory doesnt exists or is not a directory
        // TODO throw an exception
    }

    std::shared_ptr<service> srv(new service(path));

    // TODO Check it is service directory
    // TODO Load configuration if exists

    // List service directory database content
    for(auto dir_entry : fs::directory_iterator(path, ~fs::directory_options::none)) {
        if(dir_entry.exists() && dir_entry.is_directory()) {
            srv->load_database(dir_entry.path());
        }
    }

    return srv;
}

std::shared_ptr<database> service::load_database(const std::filesystem::path& path) {
    std::string name = path.filename();
    // TODO Check the database is not already loaded and not in name collision
    auto db = database::load_database(name, path);
    _databases.insert({name, db});
    return db;
}


} // cyclic::server
