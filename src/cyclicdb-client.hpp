/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-client.hpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicdb-client is free software: you can redistribute it and/or
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
#ifndef CYCLICDB_CYCLICDB_CLIENT_HPP
#define CYCLICDB_CYCLICDB_CLIENT_HPP

#include "common-base.hpp"

#include "cyclicdb-client-http.hpp"


namespace cyclic::client {

class connection;
class database;

namespace internal {
struct ts_descriptor;
} // namespace internal

/**
 * Timeseries
 */
class time_series : public cyclic::table {
private:
    std::weak_ptr<connection> _connection;
    std::string _database;
    std::string _name;

    std::unique_ptr<internal::ts_descriptor> _desc;

protected:
    friend class database;
    time_series(std::weak_ptr<connection> connection, const std::string& database, const std::string& name);

    inline void refresh_if_empty() const;

public:

    std::string name() const;

    /**
     * Refresh timeseries metadata.
     */
    void refresh();

    field_index_t field_count() const override;

    const cyclic::field &field(field_index_t field) const override;

    const cyclic::field &field(const std::string &field_name) const override;

    record_index_t record_count() const override;

    record_index_t min_index() const override;

    record_index_t max_index() const override;

    record_index_t record_capacity() const override;

    record_time_t record_origin() const override;

    record_time_t record_duration() const override;


//
// Not implemented yet, stubs only:
//

    const_recordset_iterator begin() const override;

    const_recordset_iterator end() const override;

    record_index_t record_index(record_time_t time) const override;

    record_time_t record_time(record_index_t index) const override;

    std::unique_ptr<mutable_record> get_record() const override;

    std::unique_ptr<record> get_record(record_index_t index) const override;

    std::unique_ptr<record> get_record(record_time_t time) const override;

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

/**
 * Database.
 */
class database {
private:
    std::weak_ptr<connection> _connection;
    std::string _name;

protected:
    friend class connection;
    database(std::weak_ptr<connection> connection, const std::string& name);

public:

    std::string name() const;

    /**
     * Retrieve the list of all timeseries managed by the database.
     * @return The list of timeseries.
     */
    std::vector<std::shared_ptr<time_series>> get_time_series();

    /**
     * Retrieve details of a timeseries from its name.
     * @param name Name of the timeseries to retrieve.
     * @return The timeseries.
     */
    std::shared_ptr<time_series> get_time_series(const std::string& name);

    /**
     * Create a time series.
     * @param name Name of the time series.
     * @param columns Column descriptors
     * @param capacity time series caapcity, in number of rows.
     * @return the created time series
     */
    std::shared_ptr<time_series> create_time_series(const std::string& name, const std::vector<cyclic::field_st>& columns, cyclic::record_index_t capacity);
};


/**
 * Connection to a CyclicDB server.
 */
class connection : public std::enable_shared_from_this<connection>{
private:
    std::string _url;

    std::shared_ptr<cyclic_http_client> _client;

    connection(const std::string& url);

protected:
    friend class database;
    friend class time_series;

    std::shared_ptr<cyclic_http_client> client() {return _client;}

public:

    ~connection();

    /**
     * Return the number of databases managed by the service.
     * @return Number of managed databases.
     */
    size_t get_database_count();

    /**
     * Retrieve the list of all databases managed by the service this connection is connected to.
     * @return The list of database.
     */
    std::vector<std::shared_ptr<database>> get_databases();

    /**
     * Retrieve an accessor to a specific database, from its name.
     * @param name Name of the database to look for.
     * @returns The database.
     */
    std::shared_ptr<database> get_database(const std::string& name);

    /**
     * Create a new database.
     * @param name Name of the database to create.
     * @returns The database.
     */
    std::shared_ptr<database> create_database(const std::string& name);

    /**
     * Create a new connection based on the specified URL.
     * The newly created connection is not tested.
     *
     * @param url URL of the service API to connect to.
     * @return The newly created connection
     */
    static std::shared_ptr<connection> create_connection(const std::string& url);

};

} // namespace cyclic::client
#endif //CYCLICDB_CYCLICDB_CLIENT_HPP
