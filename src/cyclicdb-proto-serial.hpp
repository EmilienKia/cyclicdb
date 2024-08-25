/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-proto-serial.hpp
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
#ifndef _CYCLIC_CYCLICDB_PROTO_SERIAL_HPP_
#define _CYCLIC_CYCLICDB_PROTO_SERIAL_HPP_

#include "cyclicdb.pb.h"
#include <google/protobuf/util/json_util.h>

#include "common-type.hpp"
#include "common-base.hpp"

#include "cyclicdb-messages.hpp"

#define CYCLIC_PROTO_DECLARE_SERIAL(SRC, DST) \
    void operator<<(SRC &src, const DST &dst); \
    void operator>>(const SRC &src, DST &dst); \
    inline void operator>>(const DST& dst, SRC& src) {return src << dst;} \
    inline void operator<<(DST& dst, const SRC& src) {return src >> dst;}



namespace cyclic::proto {


inline cyclic::proto::data_type field_type_to_proto_type(cyclic::data_type type) {
    return (cyclic::proto::data_type)(type+1);
}

inline cyclic::data_type proto_type_to_field_type(cyclic::proto::data_type type) {
    return (cyclic::data_type)(type-1);
}


struct set_data_value_visitor
{
    cyclic::proto::DataValue& data_value;
    void operator()(std::monostate){data_value.clear_value();}
    void operator()(bool val)const{data_value.set_boolean(val);}
    void operator()(int8_t val)const{data_value.set_int8(val);}
    void operator()(uint8_t val)const{data_value.set_uint8(val);}
    void operator()(int16_t val)const{data_value.set_int16(val);}
    void operator()(uint16_t val)const{data_value.set_uint16(val);}
    void operator()(int32_t val)const{data_value.set_int32(val);}
    void operator()(uint32_t val)const{data_value.set_uint32(val);}
    void operator()(int64_t val)const{data_value.set_int64(val);}
    void operator()(uint64_t val)const{data_value.set_uint64(val);}
    void operator()(float val)const{data_value.set_float_(val);}
    void operator()(double val)const{data_value.set_double_(val);}
};

cyclic::proto::DataValue& operator << (cyclic::proto::DataValue& data, const cyclic::value_t& value);
const cyclic::proto::DataValue& operator >> (const cyclic::proto::DataValue& data, cyclic::value_t& value);


CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::Error, error_report);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::ServiceStatusResponse, service_status_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::ListDatabasesResponse, list_databases_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::CreateDatabaseRequest, create_database_request);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::CreateDatabaseResponse, create_database_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::GetDatabaseResponse, get_database_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::ListTimeseriesResponse, list_timeseries_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::CreateTimeseriesRequest, create_timeseries_request);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::CreateTimeseriesResponse, create_timeseries_response);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::GetTimeseriesResponse, timeseries_details);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::GetTimeseriesDataResponse, timeseries_data);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::SetTimeseriesDataRequest, timeseries_data_set_request);
CYCLIC_PROTO_DECLARE_SERIAL(cyclic::proto::SetTimeseriesDataResponse, timeseries_data_set_response);



} // namespace cyclic::proto
#undef CYCLIC_PROTO_DECLARE_SERIAL
#endif // _CYCLIC_CYCLICDB_PROTO_SERIAL_HPP_