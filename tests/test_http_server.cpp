


#include "cyclicdb-service.hpp"

#include "cyclicdbd-server-http.hpp"
#include "cyclicdb-client-http.hpp"


#include <filesystem>
#include <random>
#include <iostream>




#if 1
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <pistache/router.h>

#include <pistache/client.h>
#include <pistache/net.h>

#include <chrono>
#endif


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




int main() {
    temp_dir dir;
    std::cout << "TEMP server : " << *dir << std::endl;

    std::vector<cyclic::field_st> fields{
            {"0bool", cyclic::CDB_DT_BOOLEAN,},
            {"1int8", cyclic::CDB_DT_SIGNED_8},
            {"2uint8", cyclic::CDB_DT_UNSIGNED_8},
            {"3int16", cyclic::CDB_DT_SIGNED_16},
            {"4uint16", cyclic::CDB_DT_UNSIGNED_16},
            {"5int32", cyclic::CDB_DT_SIGNED_32},
            {"6uint32", cyclic::CDB_DT_UNSIGNED_32},
            {"7int64", cyclic::CDB_DT_SIGNED_64},
            {"8uint64", cyclic::CDB_DT_UNSIGNED_64},
            {"9float", cyclic::CDB_DT_FLOAT_4},
            {"Adouble", cyclic::CDB_DT_FLOAT_8}
    };
    size_t capacity = 10;

    auto service = cyclic::server::service::create_service(*dir);

    auto server = std::make_shared<cyclic_server>(service);
 
    cyclic_http_server http_server{server, "*:0"};
    http_server.init();
    http_server.start();



#if 1

    auto client = cyclic::client::cyclic_http_client::create("http://localhost:" + std::to_string(http_server.get_bind_port()));

    {
        auto res = client->status();
//        auto res = client->list_database_names();
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

    http_server.shutdown();

    return 0;
}

