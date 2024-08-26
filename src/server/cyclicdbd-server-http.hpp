/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/server/cyclicdbd-server-http.hpp
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
#ifndef CYCLICDB_CYCLICDBD_SERVER_HTTP_HPP
#define CYCLICDB_CYCLICDBD_SERVER_HTTP_HPP

#include "cyclicdbd-server.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <pistache/router.h>

#include <google/protobuf/util/json_util.h>

#include "../common/cyclicdb-proto-serial.hpp"



class cyclic_http_server {
protected:
    typedef cyclic_http_server _self_;

    std::shared_ptr<Pistache::Http::Endpoint> _endpoint;
    Pistache::Rest::Router _router;

    std::shared_ptr<cyclic_server> _server;

public:

    cyclic_http_server(std::shared_ptr<cyclic_server> server);
    cyclic_http_server(std::shared_ptr<cyclic_server> server, const std::string& addr);

    void init(size_t thr = 2);
    void start();
    void shutdown();

    unsigned short get_bind_port();

private:
    void setup();

protected:

    template <typename Res, typename PBRes, typename Req, typename PBReq, typename Exec = cyclic::proto::expected<Res>(_self_::*)(const Req&)>
    void process(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter& response, Exec exec) {

        PBReq pb_req;
        google::protobuf::util::JsonStringToMessage(request.body(), &pb_req);

        Req req;
        (const PBReq&)pb_req >> (Req&)req;

        cyclic::proto::expected<Res> res = exec(req);

        if(cyclic::proto::succeed(res)) {
            PBRes pb_res;
            pb_res << cyclic::proto::value(res);

            std::string str;
            google::protobuf::util::MessageToJsonString(pb_res, &str);
            response.send(Pistache::Http::Code::Ok, str, MIME(Application, Json));
            std::clog << "S> POST responds:" << str << std::endl;
        } else {
            cyclic::proto::Error e;
            e << cyclic::proto::error(res);
            std::string str;
            google::protobuf::util::MessageToJsonString(e, &str);
            response.send((Pistache::Http::Code)cyclic::proto::error(res).category, str, MIME(Application, Json));
            std::clog << "S> POST reponds (error):" << str << std::endl;
        }
    }

    template <typename Res, typename PBRes, typename Exec = cyclic::proto::expected<Res>(_self_::*)()>
    void process(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter& response, Exec exec) {

        cyclic::proto::expected<Res> res = exec();

        if(cyclic::proto::succeed(res)) {
            PBRes pb_res;
            pb_res << cyclic::proto::value(res);

            std::string str;
            google::protobuf::util::MessageToJsonString(pb_res, &str);
            response.send(Pistache::Http::Code::Ok, str, MIME(Application, Json));
            std::clog << "S> GET responds:" << str << std::endl;
        } else {
            cyclic::proto::Error e;
            e << cyclic::proto::error(res);
            std::string str;
            google::protobuf::util::MessageToJsonString(e, &str);
            response.send((Pistache::Http::Code) cyclic::proto::error(res).category, str, MIME(Application, Json));
            std::clog << "S> GET responds(error):" << str << std::endl;
        }
    }

public:
    static void handle_home(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_status(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_list_databases(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_create_database(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_get_database(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_list_timeseries(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_create_timeseries(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_get_timeseries(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_get_timeseries_data(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    void handle_set_timeseries_data(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
};


#endif // CYCLICDB_CYCLICDBD_SERVER_HTTP_HPP
