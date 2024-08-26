/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/server/cyclicdbd-server.hpp
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

#ifndef CYCLICDB_CYCLICDBD_SERVER_HPP
#define CYCLICDB_CYCLICDBD_SERVER_HPP

#include "cyclicdb-service.hpp"

#include "../common/cyclicdb-messages.hpp"

#include <optional>

class cyclic_server {
protected:
    std::shared_ptr<cyclic::server::service> _service;

public:
    cyclic_server(std::shared_ptr<cyclic::server::service> service) : _service(service) {}

    cyclic::proto::expected<cyclic::proto::service_status_response> status();

    cyclic::proto::expected<cyclic::proto::list_databases_response> list_database_names();

    cyclic::proto::expected<cyclic::proto::create_database_response> create_database(const std::string& dbname);

    cyclic::proto::expected<cyclic::proto::get_database_response> get_database(const std::string& ndbame);

    cyclic::proto::expected<cyclic::proto::list_timeseries_response> list_timeseries(const std::string& dbname);

    cyclic::proto::expected<cyclic::proto::create_timeseries_response> create_timeseries(const std::string& dbname, const cyclic::proto::create_timeseries_request& req);

    cyclic::proto::expected<cyclic::proto::timeseries_details> get_timeseries_details(const std::string& dbname, const std::string& tsname);

    cyclic::proto::expected<cyclic::proto::timeseries_data> get_timeseries_data(const std::string& dbname, const std::string& tsname, std::optional<cyclic::record_index_t> start, std::optional<cyclic::record_index_t> end, std::optional<cyclic::record_index_t> count);

    cyclic::proto::expected<cyclic::proto::timeseries_data_set_response> set_timeseries_data(const std::string& dbname, const std::string& tsname, const cyclic::proto::timeseries_data_set_request& data);
};



#endif // CYCLICDB_CYCLICDBD_SERVER_HPP
