/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-client-http.cpp
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

#include "cyclicdb-client-http.hpp"

#include "cyclicdb.pb.h"
#include "cyclicdb-proto-serial.hpp"


#include <pistache/client.h>
#include <pistache/http.h>
#include <pistache/net.h>


using namespace Pistache;
using namespace cyclic::proto;

namespace cyclic::client {

class cyclic_pistache_http_client : public cyclic_http_client {
protected:
    Pistache::Http::Experimental::Client _client;

    http_response get(const http_get& req) override;
    http_response post(const http_post& req) override;

public:
    cyclic_pistache_http_client(const std::string& url);
    virtual ~cyclic_pistache_http_client();
};





//
// cyclic_http_client
//

std::shared_ptr<cyclic_http_client> cyclic_http_client::create(const std::string& url) {
    return std::shared_ptr<cyclic_http_client>{new cyclic_pistache_http_client(url)};
}

cyclic_http_client::cyclic_http_client(const std::string& url):
_url(url)
{
}


// Pistache::Http::Code

template<typename Res, typename PBRes>
cyclic::proto::expected<Res> cyclic_http_client::get(const std::string& path, /*std::initializer_list<std::pair<const std::string, std::string>>*/ const std::map<std::string,std::string>& params) {

    http_get request{
        .path = path,
        .accept = "application/json",
        .params = params
    };

    http_response response = get(request);
    if(response.code == (unsigned short)Http::Code::Ok) {
        PBRes pb_res;
        google::protobuf::util::JsonStringToMessage(response.content, &pb_res);
        Res res;
        res << pb_res;
        return res;
    } else {
        cyclic::proto::Error error;
        google::protobuf::util::JsonStringToMessage(response.content, &error);
        cyclic::proto::error_report err;
        err << error;
        return err;
    }

}

template<typename Res, typename PBRes, typename Req, typename PBReq>
cyclic::proto::expected<Res> cyclic_http_client::post(const std::string& path, const Req& req) {
    http_post request{
        .path = path,
        .accept = "application/json",
        .content_type = "application/json",
    };

    PBReq pb_request;
    pb_request << req;
    google::protobuf::util::MessageToJsonString(pb_request, &request.content);

    http_response response = post(request);
    if(response.code == (unsigned short)Http::Code::Ok) {
        PBRes pb_res;
        google::protobuf::util::JsonStringToMessage(response.content, &pb_res);
        Res res;
        res << pb_res;
        return res;
    } else {
        cyclic::proto::Error error;
        google::protobuf::util::JsonStringToMessage(response.content, &error);
        cyclic::proto::error_report err;
        err << error;
        return err;
    }

}



cyclic::proto::expected<cyclic::proto::service_status_response> cyclic_http_client::status() {
    return get<service_status_response, ServiceStatusResponse>("/status");
}

cyclic::proto::expected<cyclic::proto::list_databases_response> cyclic_http_client::list_database_names() {
    return get<list_databases_response, ListDatabasesResponse>("/api/v1/databases");
}

cyclic::proto::expected<cyclic::proto::create_database_response> cyclic_http_client::create_database(const std::string& dbname) {
    cyclic::proto::create_database_request request{.dbname = dbname};
    return post<create_database_response, CreateDatabaseResponse, create_database_request, CreateDatabaseRequest>("/api/v1/databases", request);
}

cyclic::proto::expected<cyclic::proto::get_database_response> cyclic_http_client::get_database(const std::string& dbame) {
    return get<get_database_response, GetDatabaseResponse>("/api/v1/databases/" + dbame);
}

cyclic::proto::expected<cyclic::proto::list_timeseries_response> cyclic_http_client::list_timeseries(const std::string& dbname) {
    return get<list_timeseries_response, ListTimeseriesResponse>("/api/v1/databases/" + dbname + "/timeseries");
}

cyclic::proto::expected<cyclic::proto::create_timeseries_response> cyclic_http_client::create_timeseries(const std::string& dbname, const cyclic::proto::create_timeseries_request& req) {
    return post<create_timeseries_response, CreateTimeseriesResponse, create_timeseries_request, CreateTimeseriesRequest>("/api/v1/databases/" + dbname + "/timeseries", req);
}

cyclic::proto::expected<cyclic::proto::timeseries_details> cyclic_http_client::get_timeseries_details(const std::string& dbname, const std::string& tsname) {
    return get<timeseries_details, GetTimeseriesResponse>("/api/v1/databases/" + dbname + "/timeseries/" + tsname);
}

cyclic::proto::expected<cyclic::proto::timeseries_data> cyclic_http_client::get_timeseries_data(const std::string& dbname, const std::string& tsname, std::optional<cyclic::record_index_t> start, std::optional<cyclic::record_index_t> end) {
    std::map<std::string, std::string> params;
    if(start) {
        params["start"] = std::to_string(*start);
    }
    if(end) {
        params["end"] = std::to_string(*end);
    }
    return get<timeseries_data, GetTimeseriesDataResponse>("/api/v1/databases/" + dbname + "/timeseries/" + tsname + "/data", params);
}

cyclic::proto::expected<cyclic::proto::timeseries_data_set_response> cyclic_http_client::set_timeseries_data(const std::string& dbname, const std::string& tsname, const cyclic::proto::timeseries_data_set_request& data) {
    return post<timeseries_data_set_response, SetTimeseriesDataResponse, timeseries_data_set_request, SetTimeseriesDataRequest>("/api/v1/databases/" + dbname + "/timeseries/" + tsname  + "/data", data);
}





//
// cyclic_pistache_http_client
//

cyclic_pistache_http_client::cyclic_pistache_http_client(const std::string& url) :
cyclic_http_client(url)
{
    auto opts = Pistache::Http::Experimental::Client::options().threads(1).maxConnectionsPerHost(8);
    _client.init(opts);
}

cyclic_pistache_http_client::~cyclic_pistache_http_client()
{
    _client.shutdown();
}

cyclic_http_client::http_response cyclic_pistache_http_client::get(const http_get& req) {
    using namespace Pistache;
    static const std::string accept_src = "application/json";
    auto accept = std::make_shared<Http::Header::Accept>();
    accept->parseRaw(accept_src.data(), accept_src.size());

    Http::Uri::Query query;
    for(const auto& param : req.params) {
        query.add(param.first, param.second);
    }

    auto promise = _client.get(_url + req.path)
        .params(query)
//        .header(accept)
        .send();

    std::clog << "C> Request GET " << _url + req.path << std::endl;

    Http::Response resp;
    std::exception_ptr except;
    promise.then([&](Http::Response response){
        std::clog << "C> GET Response: " << response.body() << std::endl;
        resp = std::move(response);
    },[&](std::exception_ptr ex){
        std::clog << "C> GET Response: exception" << std::endl;
        except = std::move(ex);
    });
    Async::Barrier<Http::Response> barrier{promise};
    barrier.wait();

    if(except) {
        std::cerr << "exception" << std::endl;
        std::rethrow_exception(except);
    } else {
        return cyclic_http_client::http_response{
            .code = (unsigned short) resp.code(),
            .content = resp.body()
        };
    }
}

cyclic_http_client::http_response cyclic_pistache_http_client::post(const http_post& req) {
    using namespace Pistache;
    static const std::string accept_src = "application/json";
    auto accept = std::make_shared<Http::Header::Accept>();
    accept->parseRaw(accept_src.data(), accept_src.size());

    auto promise = _client.post(_url + req.path)
        .body(req.content)
//        .header(Http::Header::ContentType(req.content_type))
//        .header(accept)
        .send();

    std::clog << "C> Request POST " << _url + req.path << std::endl;

    Http::Response resp;
    std::exception_ptr except;
    promise.then([&](Http::Response response){
        std::clog << "C> Response: " << response.body() << std::endl;
        resp = std::move(response);
    },[&](std::exception_ptr ex){
        std::clog << "C> Response: exception" << std::endl;
        except = std::move(ex);
    });
    Async::Barrier<Http::Response> barrier{promise};
    barrier.wait();

    if(except) {
        std::cerr << "exception" << std::endl;
        std::rethrow_exception(except);
    } else {
        return cyclic_http_client::http_response{
            .code = (unsigned short) resp.code(),
            .content = resp.body()
        };
    }
}



} // namespace cyclic::client
