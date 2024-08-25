/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdb-messages.hpp
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
#ifndef CYCLICDB_CYCLICDB_MESSAGES_HPP
#define CYCLICDB_CYCLICDB_MESSAGES_HPP

#include "common-base.hpp"

#include <map>
#include <string>
#include <variant>
#include <optional>
#include <iostream>

namespace cyclic::proto {


struct error_report {
    unsigned short category;
    unsigned long id;

    struct error_context {
        std::string message;
        typedef std::variant<std::string, bool, int, uint32_t, float, double, error_context> param;
        std::map<std::string, param> params;
    };

    error_context context;
};


template<typename Type>
using expected = std::variant<error_report, Type>;

template<typename Type>
inline bool succeed(const expected<Type>& exp) {
    return !std::holds_alternative<error_report>(exp);
}

template<typename Type>
inline Type& value(expected<Type>& exp) {
    return std::get<Type>(exp);
}

template<typename Type>
inline const Type& value(const expected<Type>& exp) {
    return std::get<Type>(exp);
}

template<typename Type>
inline error_report& error(expected<Type>& exp) {
    return std::get<error_report>(exp);
}

template<typename Type>
inline const error_report& error(const expected<Type>& exp) {
    return std::get<error_report>(exp);
}


struct service_status_response {
    bool status;
};

struct list_databases_response {
    std::vector<std::string> databases;
};

struct create_database_request {
    std::string dbname;
};

struct create_database_response {
    std::string dbname;
};

struct get_database_response {
    std::string dbname;
};

struct list_timeseries_response {
    std::vector<std::string> timeseries;
};

struct create_timeseries_request {
    std::string tsname;
    std::vector<cyclic::field_st> fields;
    cyclic::record_index_t capacity;
    cyclic::record_time_t origin = 0;
    cyclic::record_time_t duration = 0;
};

struct create_timeseries_response {
    std::string tsname;
};

struct timeseries_details {
    std::string tsname;
    cyclic::field_index_t field_count;
    std::vector<cyclic::field_st> fields;

    cyclic::record_index_t record_capacity;
    cyclic::record_time_t record_origin;
    cyclic::record_time_t record_duration;

    cyclic::record_index_t record_count;

    std::optional<cyclic::record_index_t> min_index;
    std::optional<cyclic::record_index_t> max_index;
};

struct timeseries_record_data {
    std::optional<cyclic::record_index_t> record_index;
    std::optional<cyclic::record_time_t> record_time;
    std::map<cyclic::field_index_t, cyclic::value_t> values;
};

struct timeseries_data {
    std::string tsname;
    cyclic::field_index_t field_count;
    std::vector<cyclic::field_st> fields;

    cyclic::record_index_t record_count;
    std::vector<timeseries_record_data> records;
};

struct timeseries_data_set_request {
    std::map<cyclic::field_index_t, std::string> field_mapping;
    std::vector<timeseries_record_data> records;
};

struct timeseries_data_set_response {
    std::string tsname;
};





// Template stream operator for error_report to ostream
template<typename STM>
inline STM& operator<<(STM& os, const error_report& err) {
    os << err.category << ":" << err.id << " : " << err.context.message << std::endl;
    return os;
}


} // namespace cyclic::proto
#endif //CYCLICDB_CYCLICDB_MESSAGES_HPP
