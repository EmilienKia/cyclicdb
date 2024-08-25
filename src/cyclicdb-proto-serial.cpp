/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-proto-serial.cpp
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
#include "cyclicdb-proto-serial.hpp"

namespace cyclic::proto {


cyclic::proto::DataValue& operator << (cyclic::proto::DataValue& data, const cyclic::value_t& value) {
    set_data_value_visitor visitor{.data_value = data};
    std::visit(visitor, value);
    return data;
}

const cyclic::proto::DataValue& operator >> (const cyclic::proto::DataValue& data, cyclic::value_t& value) {
    switch(data.value_case()) {
        case cyclic::proto::DataValue::kBoolean:
            value = data.boolean();
            break;
        case cyclic::proto::DataValue::kInt8:
            value = (int8_t)data.int8();
            break;
        case cyclic::proto::DataValue::kUint8:
            value = (uint8_t)data.uint8();
            break;
        case cyclic::proto::DataValue::kInt16:
            value = (int16_t)data.int16();
            break;
        case cyclic::proto::DataValue::kUint16:
            value = (uint16_t)data.uint16();
            break;
        case cyclic::proto::DataValue::kInt32:
            value = (int32_t)data.int32();
            break;
        case cyclic::proto::DataValue::kUint32:
            value = (uint32_t)data.uint32();
            break;
        case cyclic::proto::DataValue::kInt64:
            value = (int64_t)data.int64();
            break;
        case cyclic::proto::DataValue::kUint64:
            value = (uint64_t)data.uint64();
            break;
        case cyclic::proto::DataValue::kFloat:
            value = (float)data.float_();
            break;
        case cyclic::proto::DataValue::kDouble:
            value = (double)data.double_();
            break;
        case cyclic::proto::DataValue::VALUE_NOT_SET:
            value.reset();
            break;
    }
    return data;
}




//
// cyclic::proto::Error <=> error_report
//

void operator<<(cyclic::proto::Error &error, const error_report &err) {
    error.set_id(err.id);
    // TODO propagate message context
    error.mutable_context()->set_message(err.context.message);
}

void operator>>(const cyclic::proto::Error &error, error_report &err) {
    err.id = error.id();
    // TODO propagate message context
    err.context.message = error.context().message();
}

//
// cyclic::proto::ServiceStatusResponse <=> service_status_response
//

void operator<<(cyclic::proto::ServiceStatusResponse &res, const service_status_response &status) {
    res.set_status(status.status ? "OK" : "Failure");
}

void operator>>(const cyclic::proto::ServiceStatusResponse &res, service_status_response &status) {
    status.status = res.status() == "OK";
}

//
// cyclic::proto::ListDatabasesResponse <=> list_databases_response
//

void operator<<(cyclic::proto::ListDatabasesResponse &res, const list_databases_response &response) {
    for (auto &name: response.databases) {
        res.add_databases(name);
    }
}

void operator>>(const cyclic::proto::ListDatabasesResponse &res, list_databases_response &response) {
    for (int i = 0; i < res.databases_size(); i++) {
        response.databases.push_back(res.databases(i));
    }
}

//
// cyclic::proto::CreateDatabaseRequest <=> create_database_request
//

void operator<<(cyclic::proto::CreateDatabaseRequest& req, const create_database_request& request) {
    req.set_name(request.dbname);
}

void operator>>(const cyclic::proto::CreateDatabaseRequest& req, create_database_request& request) {
    request.dbname = req.name();
}

//
// cyclic::proto::CreateDatabaseResponse <=> create_database_response
//

void operator<<(cyclic::proto::CreateDatabaseResponse &res, const create_database_response &response) {
    res.set_name(response.dbname);
}

void operator>>(const cyclic::proto::CreateDatabaseResponse &res, create_database_response &response) {
    response.dbname = res.name();
}

//
// cyclic::proto::GetDatabaseResponse <=> get_database_response
//

void operator <<(cyclic::proto::GetDatabaseResponse &res, const get_database_response& response) {
    res.set_name(response.dbname);
}

void operator >>(const cyclic::proto::GetDatabaseResponse& res, get_database_response& response) {
    response.dbname = res.name();
}

//
// cyclic::proto::ListTimeseriesResponse <=> list_timeseries_response
//

void operator<<(cyclic::proto::ListTimeseriesResponse &res, const list_timeseries_response &req) {
    for (auto &name: req.timeseries) {
        res.add_timeseries(name);
    }
}

void operator>>(const cyclic::proto::ListTimeseriesResponse &res, list_timeseries_response &req) {
    for (int i = 0; i < res.timeseries_size(); i++) {
        req.timeseries.push_back(res.timeseries(i));
    }
}

//
// cyclic::proto::CreateTimeseriesRequest <=> create_timeseries_request
//

void operator<<(cyclic::proto::CreateTimeseriesRequest& req, const create_timeseries_request& request) {
    req.set_name(request.tsname);
    for(auto field : request.fields) {
        auto f = req.add_columns();
        f->set_name(field.name);
        f->set_type(field_type_to_proto_type(field.type));
    }
    req.set_capacity(request.capacity);
    if(request.origin)
        req.set_origin(request.origin);
    if(request.duration)
        req.set_duration(request.duration);
}

void operator>>(const cyclic::proto::CreateTimeseriesRequest& req, create_timeseries_request& request) {
    request.tsname = req.name();
    for(auto field : req.columns()) {
        request.fields.push_back(cyclic::field_st{field.name(), proto_type_to_field_type(field.type())});
    }
    request.capacity = req.capacity();
    request.origin = req.has_origin() ?  req.origin() : 0;
    request.duration = req.has_duration() ? req.duration() : 0;
}

//
// cyclic::proto::CreateTimeseriesResponse <=> create_timeseries_response
//

void operator<<(cyclic::proto::CreateTimeseriesResponse& res, const create_timeseries_response& response) {
    res.set_name(response.tsname);
}

void operator>>(const cyclic::proto::CreateTimeseriesResponse& res, create_timeseries_response& response) {
    response.tsname = res.name();
}

//
// cyclic::proto::GetTimeseriesResponse <=> timeseries_details
//

void operator<<(cyclic::proto::GetTimeseriesResponse& res , const timeseries_details& details) {
    res.set_name(details.tsname);
    res.set_field_count(details.field_count);

    for(cyclic::field_index_t idx = 0; idx<details.fields.size(); idx++) {
        auto field = res.add_fields();
        field->set_index(idx);
        field->set_name(details.fields[idx].name);
        field->set_type(cyclic::proto::field_type_to_proto_type(details.fields[idx].type));
    }

    res.set_record_capacity(details.record_capacity);
    res.set_record_origin(details.record_origin);
    res.set_record_duration(details.record_duration);
    res.set_record_count(details.record_count);
    if(details.min_index) {
        res.set_min_index(*details.min_index);
    }
    if(details.max_index) {
        res.set_max_index(*details.max_index);
    }
}

void operator>>(const cyclic::proto::GetTimeseriesResponse& res, timeseries_details& details) {
    details.tsname = res.name();
    details.field_count = res.field_count();
    for(int idx = 0; idx < res.fields_size(); idx++) {
        auto field = res.fields(idx);
        details.fields.push_back({field.name(), proto_type_to_field_type(field.type())});
    }
    details.record_capacity = res.record_capacity();
    details.record_origin = res.record_origin();
    details.record_duration = res.record_duration();
    details.record_count = res.record_count();
    if(res.has_min_index()) {
        details.min_index = res.min_index();
    }
    if(res.has_max_index()) {
        details.max_index = res.max_index();
    }
}

//
// cyclic::proto::GetTimeseriesDataResponse <=> timeseries_data
//

void operator<<(cyclic::proto::GetTimeseriesDataResponse& res , const timeseries_data& data) {
    res.set_name(data.tsname);
    res.set_field_count(data.field_count);

    for(cyclic::field_index_t idx = 0; idx < data.field_count; idx++) {
        const auto& field = data.fields[idx];
        auto def = res.mutable_fields()->Add();
        def->set_index(idx);
        def->set_name(field.name);
        def->set_type(cyclic::proto::field_type_to_proto_type(field.type));
    }

    for(auto& rec : data.records) {
        auto record = res.add_records();
        if(rec.record_index)
            record->set_record_index(*rec.record_index);
        if(rec.record_index)
            record->set_record_time(*rec.record_time);
        for(auto& val : rec.values) {
            cyclic::proto::DataValue data_val;
            data_val << val.second;
            record->mutable_values()->insert({val.first, data_val});
        }
    }
}

void operator>>(const cyclic::proto::GetTimeseriesDataResponse& res, timeseries_data& data) {
    data.tsname = res.name();
    data.field_count = res.field_count();
    for(int idx = 0; idx < res.fields_size(); idx++) {
        auto field = res.fields(idx);
        data.fields.push_back({field.name(), proto_type_to_field_type(field.type())});
    }
    data.record_count = res.records_size();
    for(int idx = 0; idx < res.records_size(); idx++) {
        const auto& rec = res.records(idx);
        timeseries_record_data data_rec;
        if(rec.has_record_index())
            data_rec.record_index = rec.record_index();
        if(rec.has_record_time())
            data_rec.record_time = rec.record_time();
        for(auto& val : rec.values()) {
            cyclic::value_t value;
            val.second >> value;
            data_rec.values.insert({val.first, value});
        }
        data.records.push_back(data_rec);
    }
}

//
// cyclic::proto::SetTimeseriesDataRequest <=> timeseries_data_set_request
//

void operator<<(cyclic::proto::SetTimeseriesDataRequest& req, const timeseries_data_set_request& request) {
    for(auto& field : request.field_mapping) {
        req.mutable_fields()->insert({field.first, field.second});
    }
    for(auto& rec : request.records) {
        auto record = req.add_records();
        if(rec.record_index)
            record->set_record_index(*rec.record_index);
        if(rec.record_time)
            record->set_record_time(*rec.record_time);
        for(auto& val : rec.values) {
            cyclic::proto::DataValue data_val;
            data_val << val.second;
            record->mutable_values()->insert({val.first, data_val});
        }
    }
}

void operator>>(const cyclic::proto::SetTimeseriesDataRequest& req, timeseries_data_set_request& request) {
    for(auto& field : req.fields()) {
        request.field_mapping.insert({ field.first, field.second });
    }
    for(auto& rec : req.records()) {
        timeseries_record_data data;
        if(rec.has_record_index())
            data.record_index = rec.record_index();
        if(rec.has_record_time())
            data.record_time = rec.record_time();
        for(auto& value : rec.values()) {
            cyclic::value_t val;
            value.second >> val;
            data.values.insert({value.first, val});
        }
        request.records.push_back(data);
    }
}

//
// cyclic::proto::SetTimeseriesDataResponse <=> timeseries_data_set_response
//

void operator<<(cyclic::proto::SetTimeseriesDataResponse& res, const timeseries_data_set_response& response) {
    res.set_name(response.tsname);
}

void operator>>(const cyclic::proto::SetTimeseriesDataResponse& res, timeseries_data_set_response& response) {
    response.tsname = res.name();
}




} // namespace cyclic::proto
