/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdbd-server.cpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicdbd is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * cyclicdb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the COPYING file at the root of the source distribution for more details.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cyclicdbd-server.hpp"


using namespace cyclic::proto;


//
// Global server
//

expected<service_status_response> cyclic_server::status() {
    return service_status_response{.status = (bool)_service};
}

expected<list_databases_response> cyclic_server::list_database_names() {
    return list_databases_response{.databases = _service->list_database_names()};
}

expected<create_database_response> cyclic_server::create_database(const std::string& dbname) {
    if(_service->create_database(dbname))
        return create_database_response{.dbname = dbname};
    else
        return error_report{500, 1, "Cannot create database {dbname}", {{"dbname", dbname}}};
}

expected<get_database_response> cyclic_server::get_database(const std::string& dbname) {
    if(_service->get_database(dbname))
        return get_database_response{.dbname = dbname};
    else
        return error_report{404, 1, "Database '{dbname}' is not found", {{"dbname", dbname}}};
}

expected<list_timeseries_response> cyclic_server::list_timeseries(const std::string& dbname) {
    auto db = _service->get_database(dbname);
    if(!db) {
        return error_report{404, 2, "Database '{dbname}' is not found", {{"dbname", dbname}}};
    }
    return list_timeseries_response{.timeseries=db->list_time_series_names()};
}

expected<create_timeseries_response> cyclic_server::create_timeseries(const std::string& dbname, const create_timeseries_request& req) {
    auto db = _service->get_database(dbname);
    if(!db) {
        return error_report{404, 3, "Database '{dbname}' is not found", {{"dbname", dbname}}};
    }

    if(db->get_time_series(req.tsname)) {
        return error_report{409, 4, "Timeseries '{dbname}@{tsname}' already exists", {{"dbname", dbname}, {"tsname", req.tsname}}};
    }

    auto ts = db->create_time_series(req.tsname, req.fields, req.capacity , req.origin, req.duration);
    if(ts) {
        return create_timeseries_response{.tsname=req.tsname};
    } else {
        return  error_report{500, 5, "Unknown error when trying to create timeseries '{dbname}@{tsname}'", {{"dbname", dbname}, {"tsname", req.tsname}}};
    }

}

expected<timeseries_details> cyclic_server::get_timeseries_details(const std::string& dbname, const std::string& tsname) {
    auto db = _service->get_database(dbname);
    if(!db) {
        return error_report{404, 6, "Database '{dbname}' is not found", {{"dbname", dbname}}};
    }
    auto ts = db->get_time_series(tsname);
    if(!ts) {
        return error_report{404, 7, "Timeseries '{dbname}.{tsname}' is not found", {{"dbname", dbname}, {"tsname", tsname}}};
    }

    timeseries_details res;
    res.tsname = tsname;
    res.field_count = ts->field_count();

    for(cyclic::field_index_t idx = 0; idx < ts->field_count(); idx++) {
        res.fields.push_back(cyclic::field_st{.name=ts->field(idx).name(), .type=ts->field(idx).type()} );
    }

    res.record_capacity = ts->record_capacity();
    res.record_origin = ts->record_origin();
    res.record_duration = ts->record_duration();
    res.record_count = ts->record_count();
    if(ts->min_index()!=cyclic::record::invalid_index()) {
        res.min_index = ts->min_index();
    }
    if(ts->max_index()!=cyclic::record::invalid_index()) {
        res.max_index = ts->max_index();
    }
    return res;
}

expected<timeseries_data> cyclic_server::get_timeseries_data(const std::string& dbname, const std::string& tsname, std::optional<cyclic::record_index_t> start, std::optional<cyclic::record_index_t> end) {
    auto db = _service->get_database(dbname);
    if(!db) {
        return error_report{404, 8, "Database '{dbname}' is not found", {{"dbname", dbname}}};
    }
    auto ts = db->get_time_series(tsname);
    if(!ts) {
        return error_report{404, 9, "Timeseries '{dbname}.{tsname}' is not found", {{"dbname", dbname}, {"tsname", tsname}}};
    }

    if(!start) {
        start = ts->min_index();
    }
    if(!end) {
        end = ts->max_index();
    }

    timeseries_data res;
    res.tsname = tsname;
    res.field_count = ts->field_count();
    for(cyclic::field_index_t idx = 0; idx < ts->field_count(); idx++) {
        const auto& field = ts->field(idx);
        res.fields.push_back(cyclic::field_st{.name = field.name(), .type = field.type()});
    }

    for(cyclic::record_index_t ridx = *start; ridx <= *end; ++ridx) {
        auto rec = ts->get_record(ridx);
        timeseries_record_data data{.record_index = ridx};
        if(ts->record_duration()!=0) {
            data.record_time = ts->record_time(ridx);
        }

        for (cyclic::field_index_t fidx = 0; fidx < ts->field_count(); fidx++) {
            data.values.insert({fidx, rec->get(fidx)});
        }
        res.records.push_back(std::move(data));
    }
    return res;
}

expected<timeseries_data_set_response> cyclic_server::set_timeseries_data(const std::string& dbname, const std::string& tsname, const timeseries_data_set_request& data) {
    auto db = _service->get_database(dbname);
    if(!db) {
        return error_report{404, 10, "Database '{dbname}' is not found", {{"dbname", dbname}}};
    }
    auto ts = db->get_time_series(tsname);
    if(!ts) {
        return error_report{404, 11, "Timeseries '{dbname}.{tsname}' is not found", {{"dbname", dbname}, {"tsname", tsname}}};
    }

    // TODO Add field index mapping

    for(auto& record : data.records) {
        if(record.record_index) {
            auto rec = ts->get_record();
            for (auto &value: record.values) {
                rec->set((cyclic::field_index_t) value.first, value.second);
            }
            ts->insert_record( *record.record_index, *rec);
        }
        // TODO support also time in addition to index
    }

    return timeseries_data_set_response{.tsname=tsname};
}
