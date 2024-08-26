/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/server/cyclicdb-service.hpp
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

#ifndef CYCLIC_SERVICE_HPP
#define CYCLIC_SERVICE_HPP

#include "../common/common-base.hpp"

#include <memory>
#include <map>
#include <string>
#include <filesystem>

namespace cyclic::server {

class database;
class service;


class time_series : public cyclic::table {
    friend class database;
    time_series(std::unique_ptr<cyclic::table> table);

    /// Real table delegation
    std::shared_ptr<cyclic::table> _table;

    static std::shared_ptr<time_series> load_time_series(const std::filesystem::path& path);

public:

    field_index_t field_count() const override;

    const cyclic::field &field(field_index_t field) const override;

    const cyclic::field &field(const std::string &field_name) const override;

    record_index_t record_count() const override;

    record_index_t min_index() const override;

    record_index_t max_index() const override;

    record_index_t record_capacity() const override;

    record_time_t record_origin() const override;

    record_time_t record_duration() const override;

    record_index_t record_index(record_time_t time) const override;

    record_time_t record_time(record_index_t index) const override;

    std::unique_ptr<mutable_record> get_record() const override;

    std::unique_ptr<record> get_record(record_index_t index) const override;

    std::unique_ptr<record> get_record(record_time_t time) const override;

    std::unique_ptr<recordset> get_recordset() const override;

    std::unique_ptr<recordset> get_recordset(record_index_t first, record_index_t last = record::absolute_max_index(), record_index_t count = std::numeric_limits<record_index_t>::max()) const override;

    void set_record(const record &rec) override;

    void set_record(record_index_t index, const record &rec) override;

    void set_record(record_time_t time, const record &rec) override;

    void update_record(const record &rec) override;

    void update_record(record_index_t index, const record &rec) override;

    void update_record(record_time_t time, const record &rec) override;

    void append_record() override;

    void append_record(record_index_t index) override;

    void append_record(record_time_t time) override;

    void append_record(const record &rec) override;

    void append_record(record_index_t index, const record &rec) override;

    void append_record(record_time_t time, const record &rec) override;

    void insert_record(record_index_t index) override;

    void insert_record(record_time_t time) override;

    void insert_record(const record &rec) override;

    void insert_record(record_index_t index, const record &rec) override;

    void insert_record(record_time_t time, const record &rec) override;
};

class database {
    std::string _name;
    std::filesystem::path _path;

    std::map<std::string, std::shared_ptr<time_series>> _series;

    friend class service;
    database(const std::string &name, const std::filesystem::path &path);

    /**
     * Load a database and all its content.
     * @param name Name of the database
     * @param path Path of the database directory
     * @return The just loaded database.
     */
    static std::shared_ptr<database> load_database(const std::string& name, const std::filesystem::path& path);

    /**
     * Load a time series from the specified path.
     * The time series is considered as part of this database.
     * The loaded time series is registered in this database.
     * @param path Path to look for.
     */
    std::shared_ptr<time_series> load_time_series(const std::filesystem::path& path);

public:

    /**
     * Return the number of time series managed by the database.
     * @return Number of managed time series.
     */
    size_t get_time_series_count() const;

    /**
     * Retrieve the list of time series names managed by this database.
     * @return The list of names of managed time series.
     */
    std::vector<std::string> list_time_series_names() const;

    /**
     * Retrieve an existing time series from its name.
     * @param name Name of the time series to look for.
     * @return The time series.
     */
    std::shared_ptr<time_series> get_time_series(const std::string& name) const;

    /**
     * Create a time series.
     * @param name Name of the time series
     * @param fields Field descriptors for created time series.
     * @param record_capacity Time series capacity in record number.
     * @param origin Time series time origin.
     * @param duration Time series time duration.
     * @return Created memory time series.
     * @throw std::invalid_argument Fields list is empty.
     * This is a non-sense to create a time series without fields.
     * @throw std::invalid_argument Record capacity of 0.
     * This is a non-sense to create a time series without storage capacity.
     * @throw std::invalid_argument Invalid record capacity.
     */
    std::shared_ptr<time_series> create_time_series(const std::string& name,
                                                    const std::vector<field_st>& fields,
                                                    record_index_t record_capacity,
                                                    record_time_t origin = 0, record_time_t duration = 0);


};


class service {
private:

    std::filesystem::path _path;

    std::map<std::string, std::shared_ptr<database>> _databases;

    explicit service(const std::string& path);

    /**
     * Load a database from the specified path.
     * The database is considered under control of this service.
     * The loaded database is registered in this service.
     * @param path Path to look for.
     */
    std::shared_ptr<database> load_database(const std::filesystem::path& path);


public:

    /**
     * Return the number of databases managed by the service.
     * @return Number of managed databases.
     */
    size_t get_database_count() const;

    /**
     * Create a new database in the current service.
     * @param name Name of the database.
     * @return The newly created database.
     */
    std::shared_ptr<database> create_database(const std::string& name);

    /**
     * Retrieve a database from its name.
     * @param name Name of a database to look for.
     * @return Database if exists, null otherwise.
     */
    std::shared_ptr<database> get_database(const std::string& name) const;

    /**
     * Retrieve the list of database names managed by this service.
     * @return The list of names of managed databases.
     */
    std::vector<std::string> list_database_names() const;

    /**
     * Instantiate default-configured CyclicDB service into the specified path.
     * The path must be an existing empty directory.
     * @return Newly instantiated service.
     */
    static std::shared_ptr<service> create_service(const std::filesystem::path& path);

    /**
     * Instantiate a service by loading it from filesystem.
     * @param path Path of the directory where the database storage is located.
     * @return Newly instantiated database.
     */
    static std::shared_ptr<service> load_service(const std::filesystem::path& path);

};



} // cyclic::server

#endif //CYCLIC_SERVICE_HPP
