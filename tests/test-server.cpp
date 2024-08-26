/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-seerver.cpp
 * Copyright (C) 2017-2024 Emilien Kia <emilien.kia@gmail.com>
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

#include "catch.hpp"

#include "server/cyclicdb-service.hpp"

#include "server/cyclicdbd-server-http.hpp"
#include "client/cyclicdb-client-http.hpp"


#include <filesystem>

#include <iostream>


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




TEST_CASE("Create server and database", "[server]")
{
    temp_dir dir;
    std::cout << "TEMP PATH : " << *dir << std::endl;

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
    REQUIRE( service );
    REQUIRE( service->get_database_count() == 0);

    auto database = service->create_database("test");
    REQUIRE( database );
    REQUIRE( service->get_database_count() == 1);

    auto ts = database->create_time_series("table", fields, capacity);


    auto srv = cyclic::server::service::load_service(*dir);
    REQUIRE( srv );
    REQUIRE( srv->get_database_count() == 1);

    auto db = srv->get_database("test");
    REQUIRE( db );
    REQUIRE( db->get_time_series_count() == 1 );


}





TEST_CASE("Simple end-to-end integration test", "[server][client]")
{
    temp_dir dir;
    std::cout << "TEMP PATH : " << *dir << std::endl;

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
    cyclic::record_index_t capacity = 10;

    auto service = cyclic::server::service::create_service(*dir);
    REQUIRE( service );

    auto server = std::make_shared<cyclic_server>(service);
 
    cyclic_http_server http_server{server, "*:0"};
    http_server.init();
    http_server.start();

    auto client = cyclic::client::cyclic_http_client::create("http://localhost:" + std::to_string(http_server.get_bind_port()));

    // Connect correctly
    {
        auto res = client->status();
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.status );
    }
    // No database yet
    {
        auto res = client->list_database_names();
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.databases.empty() );
    }

    // Create one database
    {
        auto res = client->create_database("test");
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.dbname == "test" );
    }
    REQUIRE( service->get_database("test") != nullptr );
    {
        auto res = client->list_database_names();
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.databases.size() == 1 );
        REQUIRE( val.databases[0] == "test" );
    }
    // No time series yet
    {
        auto res = client->list_timeseries("test");
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.timeseries.empty() );
    }

    // Create a timeseries
    {
        cyclic::proto::create_timeseries_request req {
            .tsname = "test",
            .fields = fields,
            .capacity = capacity,
            .origin = 0,
            .duration = 1
        };
        auto res = client->create_timeseries("test", req);
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.tsname == req.tsname );
    }
    // Ensure the timeseries exists now
    {
        auto res = client->list_timeseries("test");
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.timeseries.size() == 1 );
        REQUIRE( val.timeseries[0] == "test" );
    }
    // Check the structure of the timeseries
    {
        auto res = client->get_timeseries_details("test", "test");
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.tsname == "test" );
        REQUIRE( val.fields.size() == fields.size() );
        // Check fields
        for(size_t i = 0; i < fields.size(); i++) {
            REQUIRE( val.fields[i].name == fields[i].name );
            REQUIRE( val.fields[i].type == fields[i].type );
        }
    }
    // Check the timeseries is empty (0 records)
    {
        auto res = client->get_timeseries_details("test", "test");
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.record_count == 0 );
    }

    // Data to check
    std::vector<cyclic::proto::timeseries_record_data> records{
                {.record_index = 0, .record_time = 0, .values = {{0, true}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}}},
                {.record_index = 1, .record_time = 1, .values = {{0, false}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 10}}},
                {.record_index = 2, .record_time = 2, .values = {{0, true}, {1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}, {6, 8}, {7, 9}, {8, 10}, {9, 11}}},        
    };

    std::map<cyclic::field_index_t, std::string> field_mapping{
        {0, "0bool"},
        {1, "1int8"},
        {2, "2uint8"},
        {3, "3int16"},
        {4, "4uint16"},
        {5, "5int32"},
        {6, "6uint32"},
        {7, "7int64"},
        {8, "8uint64"},
        {9, "9float"},
    };

    // Put some data
    {
        cyclic::proto::timeseries_data_set_request data{.field_mapping = field_mapping, .records = records};
        auto res = client->set_timeseries_data("test", "test", data);
        REQUIRE( cyclic::proto::succeed(res) );
    }

    // Check data
    {
        auto res = client->get_timeseries_data("test", "test", cyclic::record_index_t{0}, cyclic::record_index_t{2});
        REQUIRE( cyclic::proto::succeed(res) );
        auto& val = cyclic::proto::value(res);
        REQUIRE( val.record_count == records.size() );
        for(size_t i = 0; i < records.size(); i++) {
            REQUIRE( val.records[i].record_index == records[i].record_index );
            REQUIRE( val.records[i].record_time == records[i].record_time );
            for(auto& [index, value] : records[i].values) {
                REQUIRE( equals(val.records[i].values[index], value) );
            }
        }
    }

    http_server.shutdown();

}


