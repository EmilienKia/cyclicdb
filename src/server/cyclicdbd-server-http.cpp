/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/server/cyclicdbd-server-http.cpp
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


#include "cyclicdbd-server-http.hpp"

#include "cyclicdbd-server.hpp"


using namespace cyclic::proto;

using namespace Pistache;

#define DEFAULT_PORT "9081"



//
// HTTP server
//


cyclic_http_server::cyclic_http_server(std::shared_ptr<cyclic_server> server) :
cyclic_http_server(server, "*:" DEFAULT_PORT)
{
}

cyclic_http_server::cyclic_http_server(std::shared_ptr<cyclic_server> server, const std::string& addr) :
    _server(server),
    _endpoint(std::make_shared<Pistache::Http::Endpoint>(Pistache::Address(addr)))
{
}

void cyclic_http_server::init(size_t thr)
{
    auto opts = Pistache::Http::Endpoint::options().threads(static_cast<int>(thr));
    _endpoint->init(opts);
    setup();
}

void cyclic_http_server::start()
{
    _endpoint->setHandler(_router.handler());
    _endpoint->serveThreaded();
}

void cyclic_http_server::shutdown()
{
    _endpoint->shutdown();
}

unsigned short cyclic_http_server::get_bind_port()
{
    return _endpoint->getPort();
}

void cyclic_http_server::setup()
{
    // TODO
    using namespace Pistache::Rest;
    Routes::Get(_router, "/api/v1/databases/:dbname/timeseries/:tsname/data", Routes::bind(&cyclic_http_server::handle_get_timeseries_data, this));
    Routes::Post(_router, "/api/v1/databases/:dbname/timeseries/:tsname/data", Routes::bind(&cyclic_http_server::handle_set_timeseries_data, this));
    Routes::Get(_router, "/api/v1/databases/:dbname/timeseries/:tsname", Routes::bind(&cyclic_http_server::handle_get_timeseries, this));
    Routes::Post(_router, "/api/v1/databases/:dbname/timeseries", Routes::bind(&cyclic_http_server::handle_create_timeseries, this));
    Routes::Get(_router, "/api/v1/databases/:dbname/timeseries", Routes::bind(&cyclic_http_server::handle_list_timeseries, this));
    Routes::Get(_router, "/api/v1/databases/:dbname", Routes::bind(&cyclic_http_server::handle_get_database, this));
    Routes::Get(_router, "/api/v1/databases", Routes::bind(&cyclic_http_server::handle_list_databases, this));
    Routes::Post(_router, "/api/v1/databases", Routes::bind(&cyclic_http_server::handle_create_database, this));
    Routes::Get(_router, "/status", Routes::bind(&cyclic_http_server::handle_status, this));
    Routes::Get(_router, "/", Routes::bind(&cyclic_http_server::handle_home));
}

void cyclic_http_server::handle_home(const Rest::Request&, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (home) /" << std::endl;
    response.send(Http::Code::Ok, "{'version':'0.1'}", MIME(Application, Json));
}

void cyclic_http_server::handle_status(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (status) /status" << std::endl;
    process<service_status_response, cyclic::proto::ServiceStatusResponse>(request, response, [this](){
        return _server->status();
    });
}

void cyclic_http_server::handle_list_databases(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (list db) /api/v1/databases" << std::endl;
    process<list_databases_response, cyclic::proto::ListDatabasesResponse>(request, response, [this](){
        return _server->list_database_names();
    });
}

void cyclic_http_server::handle_create_database(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle POST (create db) /api/v1/databases : " << request.body() << std::endl;
    process<create_database_response, cyclic::proto::CreateDatabaseResponse, create_database_request, cyclic::proto::CreateDatabaseRequest>(request, response, [this](const create_database_request& req){
        return _server->create_database(req.dbname);
    });
};

void cyclic_http_server::handle_get_database(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (get db) /api/v1/databases/:dbname : " << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    process<get_database_response, cyclic::proto::GetDatabaseResponse>(request, response, [this, dbname](){
        return _server->get_database(dbname);
    });
}

void cyclic_http_server::handle_list_timeseries(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (list ts) /api/v1/databases/:dbname/timeseries" << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    process<list_timeseries_response, cyclic::proto::ListTimeseriesResponse>(request, response, [this, dbname](){
        return _server->list_timeseries(dbname);
    });
}

void cyclic_http_server::handle_create_timeseries(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle POST (create ts) /api/v1/databases/:dbname/timeseries" << request.body() << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    process<create_timeseries_response, cyclic::proto::CreateTimeseriesResponse, create_timeseries_request, cyclic::proto::CreateTimeseriesRequest>(request, response, [this, dbname](const create_timeseries_request& req){
        return _server->create_timeseries(dbname, req);
    });
}

void cyclic_http_server::handle_get_timeseries(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (get ts) /api/v1/databases/:dbname/timeseries/:tsname : " << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    auto tsname = request.param(":tsname").as<std::string>();
    process<timeseries_details, cyclic::proto::GetTimeseriesResponse>(request, response, [this, dbname, tsname](){
        return _server->get_timeseries_details(dbname, tsname);
    });
}

void cyclic_http_server::handle_get_timeseries_data(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle GET (get ts data) /api/v1/databases/:dbname/timeseries/:tsname/data : " << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    auto tsname = request.param(":tsname").as<std::string>();
    std::optional<cyclic::record_index_t> start;
    std::optional<cyclic::record_index_t> end;
    std::optional<cyclic::record_index_t> count;
    if(request.query().has("start")) {
        start = (cyclic::record_index_t)std::stoul(request.query().get("start").value());
// TODO support time enumeration
//    } else if(request.hasParam("time_start")) {
//        start = request.param("time_start").as<cyclic::record_time_t>();
    }
    if(request.query().has("end")) {
        end = (cyclic::record_index_t)std::stoul(request.query().get("end").value());
// TODO support time enumeration
//    } else if(request.hasParam("time_end")) {
//        end = request.param("time_end").as<cyclic::record_time_t>();
    }
    if(request.query().has("count")) {
        count = (cyclic::record_index_t)std::stoul(request.query().get("count").value());
    }
    process<timeseries_data, cyclic::proto::GetTimeseriesDataResponse>(request, response, [this, dbname, tsname, start, end, count](){
        auto res = _server->get_timeseries_data(dbname, tsname, start, end, count);
        return res;
    });
}

void cyclic_http_server::handle_set_timeseries_data(const Rest::Request& request, Http::ResponseWriter response)
{
    std::clog << "S> Handle POST (set ts data) /api/v1/databases/:dbname/timeseries/:tsname/data : " << request.body() << std::endl;
    auto dbname = request.param(":dbname").as<std::string>();
    auto tsname = request.param(":tsname").as<std::string>();

    process<timeseries_data_set_response, cyclic::proto::SetTimeseriesDataResponse, timeseries_data_set_request, cyclic::proto::SetTimeseriesDataRequest>(request, response, [this, dbname, tsname](const timeseries_data_set_request& req){
        return _server->set_timeseries_data(dbname, tsname, req);
    });
}

