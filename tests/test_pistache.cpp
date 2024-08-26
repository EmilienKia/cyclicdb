/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-pistache.cpp
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
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <pistache/router.h>

#include <pistache/client.h>
#include <pistache/net.h>

#include <chrono>

#include <filesystem>
#include <random>
#include <iostream>


#include "cyclicdb.pb.h"
#include "common/cyclicdb-messages.hpp"

#include <google/protobuf/util/json_util.h>

#include "client/cyclicdb-client-http.hpp"

#include "common/cyclicdb-proto-serial.hpp"

#include "server/cyclicdb-service.hpp"

#define DEFAULT_PORT "9081"



static std::string random_name(size_t length) {
    static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }

    return result;
}

static std::filesystem::path get_temp_path() {
    auto temp_path = std::filesystem::temp_directory_path();
    std::filesystem::path path;
    do {
        std::string name = "cydbtest" + random_name(8);
        path = temp_path / name;
    } while(std::filesystem::exists(path));
    return path;
}

/**
 * RAII temporary directory object.
 * Ensure creating a temporary filesystem directory and remove it completely at destruction.
 */
class temp_dir {
    std::filesystem::path _path;

    static std::filesystem::path get_temp_path() {
        auto temp_path = std::filesystem::temp_directory_path();
        std::filesystem::path path;
        do {
            std::string name = "cydbtest" + random_name(8);
            path = temp_path / name;
        } while(std::filesystem::exists(path));
        return path;
    }

public:
    temp_dir() {
        _path = get_temp_path();
        std::filesystem::create_directory(_path);
    }

    ~temp_dir() {
//        std::filesystem::remove_all(_path);
    }

    const std::filesystem::path& operator*()const {
        return _path;
    }

    const std::filesystem::path* operator->()const {
        return &_path;
    }

    const std::filesystem::path& path() const {
        return _path;
    }
};












// Stub for cyclic_server
class stub_server { 
protected:
    std::shared_ptr<cyclic::server::service> _service;

public:
    stub_server(std::shared_ptr<cyclic::server::service> service) : _service(service) {}

    cyclic::proto::expected<cyclic::proto::service_status_response> status();
};

cyclic::proto::expected<cyclic::proto::service_status_response> stub_server::status() {
    std::cout << "Do status" << std::endl;
    return cyclic::proto::service_status_response{.status = true};
}




// Stub for cyclic_http_server
class stub_http_server {
protected:
    typedef stub_http_server _self_;

    std::shared_ptr<Pistache::Http::Endpoint> _endpoint;
    Pistache::Rest::Router _router;

    std::shared_ptr<stub_server> _server;

public:
    stub_http_server(std::shared_ptr<stub_server> server, const std::string& addr);
    stub_http_server(std::shared_ptr<stub_server> server);

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
            std::clog << "Res:" << str << std::endl;
        } else {
            cyclic::proto::Error e;
            e << cyclic::proto::error(res);
            std::string str;
            google::protobuf::util::MessageToJsonString(e, &str);
            response.send((Pistache::Http::Code)cyclic::proto::error(res).category, str, MIME(Application, Json));
            std::clog << "Res(error):" << str << std::endl;
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
            std::clog << "Res:" << str << std::endl;
            response.send(Pistache::Http::Code::Ok, str, MIME(Application, Json));
        } else {
            cyclic::proto::Error e;
            e << cyclic::proto::error(res);
            std::string str;
            google::protobuf::util::MessageToJsonString(e, &str);
            std::clog << "Res(error):" << str << std::endl;
            response.send((Pistache::Http::Code) cyclic::proto::error(res).category, str, MIME(Application, Json));
        }
    }

public:
    static void handle_home(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response) ;
    void handle_status(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

stub_http_server::stub_http_server(std::shared_ptr<stub_server> server):
_server(server),
_endpoint(std::make_shared<Pistache::Http::Endpoint>(Pistache::Address("*:0")))
{
}

stub_http_server::stub_http_server(std::shared_ptr<stub_server> server, const std::string& addr) :
    _server(server),
    _endpoint(std::make_shared<Pistache::Http::Endpoint>(Pistache::Address(addr)))
{
}

void stub_http_server::init(size_t thr)
{
    auto opts = Pistache::Http::Endpoint::options()
            
            .threads(static_cast<int>(thr))
            .threadsName("pistache");
    _endpoint->init(opts);
    setup();
}

void stub_http_server::start()
{
    _endpoint->setHandler(_router.handler());
    _endpoint->serveThreaded();
}

void stub_http_server::shutdown()
{
    _endpoint->shutdown();
}

unsigned short stub_http_server::get_bind_port()
{
    return _endpoint->getPort();
}

void stub_http_server::setup()
{
    // TODO
    using namespace Pistache::Rest;
    Routes::Get(_router, "/status", Routes::bind(&stub_http_server::handle_status, this));
    Routes::Get(_router, "/", Routes::bind(&stub_http_server::handle_home));

}

void stub_http_server::handle_home(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::cout << "Handle home" << std::endl;
    response.send(Pistache::Http::Code::Ok, "{'status':'OK'}", MIME(Application, Json));
}

void stub_http_server::handle_status(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::cout << "Handle status" << std::endl;
    process<cyclic::proto::service_status_response, cyclic::proto::ServiceStatusResponse>(request, response, [this](){
        return _server->status();
    });
}



int main() {
    std::cout << "Test pistache" << std::endl;

    temp_dir dir;
    std::cout << "TEMP PATH : " << *dir << std::endl;

    auto service = cyclic::server::service::create_service(*dir);
    auto server = std::make_shared<stub_server>(service);

    stub_http_server http_server{server, "*:0"};
    http_server.init(1);
    std::cout << "Server initialized" << std::endl;
    http_server.start();
    std::cout << "Server started at *:" << http_server.get_bind_port() << std::endl;


#if 1

    auto client = cyclic::client::cyclic_http_client::create("http://localhost:" + std::to_string(http_server.get_bind_port()));

    {
        auto res = client->status();
//          auto res = client->list_database_names();
    }


#else

    Pistache::Http::Experimental::Client client;
    auto client_opts = Pistache::Http::Experimental::Client::options().threads(1).maxConnectionsPerHost(8);
    client.init(client_opts);

    std::string req  = "http://localhost:" + std::to_string(http_server.get_bind_port()) + "/status/";
    std::cout << "requesting : " << req << std::endl;

    auto promise = client.get(req).send();

    std::cout << "Request sent... " << std::endl;

    Pistache::Http::Response resp;
    std::exception_ptr except;

    promise.then([&](Pistache::Http::Response response){
        std::cout << "Resp: " << response.body() << std::endl;
        resp = std::move(response);
    },[&](std::exception_ptr ex){
        std::cout << "Resp: exception" << std::endl;
        except = std::move(ex);
    });

    Pistache::Async::Barrier<Pistache::Http::Response> barrier{promise};
//    barrier.wait();
    using namespace std::literals::chrono_literals;
    barrier.wait_for(5s);

    if(except) {
        std::cerr << "exception !!" << std::endl;
        std::rethrow_exception(except);
    } else {
        std::cout << "result !!" << std::endl;
    }




#endif 

/*
    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0)
    {
        std::clog << "received signal " << signal << std::endl;
    }
    else
    {
        std::clog << "sigwait returns " << status << std::endl;
    }
*/

    http_server.shutdown();

    return 0;
}
