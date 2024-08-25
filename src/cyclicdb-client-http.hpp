/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-client-http.hpp
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
#ifndef CYCLICDB_CYCLICDB_CLIENT_HTTP_HPP
#define CYCLICDB_CYCLICDB_CLIENT_HTTP_HPP


#include "common-base.hpp"

#include "cyclicdb-messages.hpp"

namespace cyclic::client {


class cyclic_http_client {
protected:
    std::string _url;

    template<typename Res, typename PBRes>
    cyclic::proto::expected<Res> get(const std::string& path, /*std::initializer_list<std::pair<const std::string, std::string>>*/ const std::map<std::string,std::string>&  params = {});

    template<typename Res, typename PBRes, typename Req, typename PBReq>
    cyclic::proto::expected<Res> post(const std::string& path, const Req& req);


    cyclic_http_client(const std::string& url);

    struct http_get {
        std::string path;
        std::string accept;
        /*std::initializer_list<std::pair<const std::string, std::string>>*/ std::map<std::string, std::string> params;
    };

    struct http_post {
        std::string path;
        std::string accept;
        std::string content_type;
        std::string content;
    };


    struct http_response {
        unsigned short code;
        std::string content;
    };

    virtual http_response get(const http_get& req) = 0;
    virtual http_response post(const http_post& req) = 0;

public:

    static std::shared_ptr<cyclic_http_client> create(const std::string& url);

    cyclic::proto::expected<cyclic::proto::service_status_response> status();

    cyclic::proto::expected<cyclic::proto::list_databases_response> list_database_names();

    cyclic::proto::expected<cyclic::proto::create_database_response> create_database(const std::string& dbname);

    cyclic::proto::expected<cyclic::proto::get_database_response> get_database(const std::string& dbame);

    cyclic::proto::expected<cyclic::proto::list_timeseries_response> list_timeseries(const std::string& dbname);

    cyclic::proto::expected<cyclic::proto::create_timeseries_response> create_timeseries(const std::string& dbname, const cyclic::proto::create_timeseries_request& req);

    cyclic::proto::expected<cyclic::proto::timeseries_details> get_timeseries_details(const std::string& dbname, const std::string& tsname);

    cyclic::proto::expected<cyclic::proto::timeseries_data> get_timeseries_data(const std::string& dbname, const std::string& tsname, std::optional<cyclic::record_index_t> start, std::optional<cyclic::record_index_t> end);

    cyclic::proto::expected<cyclic::proto::timeseries_data_set_response> set_timeseries_data(const std::string& dbname, const std::string& tsname, const cyclic::proto::timeseries_data_set_request& data);
};



} // namespace cyclic::client
#endif // CYCLICDB_CYCLICDB_CLIENT_HTTP_HPP
