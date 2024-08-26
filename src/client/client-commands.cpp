/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/client/client-commands.cpp
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

#include "client-commands.hpp"

#include "client-parser.hpp"


namespace cyclic::client {
namespace commands {

namespace intrnal
{

/*
 * val_to_str visitor class
 */

struct to_string
{
    template<typename T>
    std::string operator()(T val)const{return std::to_string(val);}

};

template<>
std::string to_string::operator()(std::monostate val)const{return "<null>";}

template<>
std::string to_string::operator()(bool val)const{return val ? "true" : "false";}

}

std::string val_to_str(cyclic::value_t val)
{
    return std::visit(intrnal::to_string{}, (cyclic::var_value_t&) val);
}

const std::array<std::string, cyclic::CDB_DT_MAX_TYPE> field_type_canonical_names
{
        "void", // CDB_DT_VOID = 0,
        "bool", // CDB_DT_BOOLEAN,
        "signed8", // CDB_DT_SIGNED_8,
        "unsigned8", // CDB_DT_UNSIGNED_8,
        "signed16", // CDB_DT_SIGNED_16,
        "unsigned16", // CDB_DT_UNSIGNED_16,
        "signed32", // CDB_DT_SIGNED_32,
        "unsigned32", // CDB_DT_UNSIGNED_32,
        "signed64", // CDB_DT_SIGNED_64,
        "unsigned64", // CDB_DT_UNSIGNED_64,
        "float4", // CDB_DT_FLOAT_4,
        "float8", // CDB_DT_FLOAT_8,
};


//
// Database commands
//

bool database_list::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
std::clog << "Execute list databases" << std::endl;
    auto dbs = cnx.get_databases();
    if (!dbs.empty()) {
        for (auto &db: dbs) {
            std::cout << db->name() << std::endl;
        }
    } else {
        std::clog << "No database." << std::endl;
    }
    return true;
}

bool database_status::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
std::clog << "Execute get database : status " << name << std::endl;
    if(name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.get_database(name);
    if(db) {
        std::cout << db->name() << std::endl;
    } else {
        std::cerr << "Database " <<  db->name() << " not found." << std::endl;
    }
    return true;
}

bool database_create::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute create database : " << name << std::endl;
    if(name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.create_database(name);
    if(db) {
        std::cout << name << std::endl;
    } else {
        std::cerr << "Database " <<  name << " cannot be created." << std::endl;
    }
    return true;
}

//
// Timeseries commands
//

bool timeseries_list::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute timeseries list : " << db_name << std::endl;
    if(db_name.empty()) {
        db_name = ctx.db_default_name;
    }
    if(db_name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.get_database(db_name);
    if(!db) {
        std::cerr << "Database " <<  db_name << " is not found." << std::endl;
        return true;
    }

    auto tss = db->get_time_series();
    for(auto& ts : tss) {
        std::cout << ts->name() << std::endl;
    }

    return true;
}

bool timeseries_details::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute timeseries details : " << db_name << "." << ts_name << std::endl;
    if(db_name.empty()) {
        db_name = ctx.db_default_name;
    }
    if(db_name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.get_database(db_name);
    if(!db) {
        std::cerr << "Database " <<  db_name << " is not found." << std::endl;
        return true;
    }

    auto ts = db->get_time_series(ts_name);
    if(!ts) {
        std::cerr << "Timeseries " <<  ts_name << " is not found." << std::endl;
        return true;
    }

    std::cout
            << db_name << "." << ts_name << ":" << std::endl;
    std::cout
            << "Definition" << std::endl
            << "  Fields      : " << ts->field_count() << std::endl;

    for(cyclic::field_index_t f=0; f<ts->field_count(); ++f)
    {
        const auto& field = ts->field(f);
        std::cout << "     " << f << " - " << field.name() << " : " << field_type_canonical_names[field.type()] << std::endl;
    }

    std::cout
            << std::endl
            << "Usage" << std::endl
            << "  Capacity    : " << ts->record_capacity() << " records" << std::endl
            << "  Used        : " << ts->record_count() << " record(s)" << std::endl;
    if(ts->min_index()!=cyclic::record::invalid_index())
        std::cout   << "  Lower index : " << ts->min_index() << std::endl;
    else
        std::cout   << "  Lower index : <none> " << std::endl;
    if(ts->max_index()!=cyclic::record::invalid_index())
        std::cout   << "  Upper index : " << ts->max_index() << std::endl;
    else
        std::cout   << "  Upper index : <none> " << std::endl;

    return true;
}

bool timeseries_create::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute timeseries create : " << db_name << " - " << ts_name << std::endl;
    if(db_name.empty()) {
        db_name = ctx.db_default_name;
    }
    if(db_name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    if(ts_name.empty()) {
        std::clog << "Timeseries name is not provided." << std::endl;
        return false;
    }

    auto db = cnx.get_database(db_name);
    if(!db) {
        std::cerr << "Database " <<  db_name << " is not found." << std::endl;
        return false;
    }

    auto ts = db->create_time_series(ts_name, fields, capacity);
    if(ts) {
        std::clog << "Timeseries " << ts_name << " successfully created in database " << db_name << "." << std::endl;
    } else {
        std::cerr << "Filed to create timeseries " << ts_name << " in database " << db_name << "." << std::endl;
    }

    return true;
}


bool timeseries_select::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute timeseries select : " << db_name << "." << ts_name << std::endl;
    if(db_name.empty()) {
        db_name = ctx.db_default_name;
    }
    if(db_name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.get_database(db_name);
    if(!db) {
        std::cerr << "Database " <<  db_name << " is not found." << std::endl;
        return true;
    }

    auto ts = db->get_time_series(ts_name);
    if(!ts) {
        std::cerr << "Timeseries " <<  ts_name << " is not found." << std::endl;
        return true;
    }

    if(ts->record_count()==0) {
        std::clog << "Timeseries " << db_name << "." << ts_name << " is empty." << std::endl;
        return true;
    }


    cyclic::record_index_t first = ts->min_index();
    cyclic::record_index_t last = ts->max_index();

    if(start.state() == parser::helpers::position::INDEX) {
        if(start.index() > first) {
            first = start.index();
        }
    } else if(start.state() == parser::helpers::position::TIME) {
        std::clog << "Selecting by time is not supported yet." << std::endl;
    }

    if(end.state() == parser::helpers::position::INDEX) {
        if(end.index() < last) {
            last = end.index();
        }
    } else if(end.state() == parser::helpers::position::TIME) {
        std::clog << "Selecting by time is not supported yet." << std::endl;
    }

    auto recset = ts->get_recordset(first, last);
    if(recset) {
        for(const auto& rec : *recset) {
            std::cout << rec.index();
            for(size_t n=0; n<colnames.size(); ++n)
            {
                std::cout << "\t" << val_to_str(rec.get(colnames[n]));
            }
            std::cout << std::endl;
        }
    }

    return true;
}


bool timeseries_insert::execute(const execution_context& ctx, cyclic::client::connection &cnx) {
    std::clog << "Execute timeseries insert : " << db_name << "." << ts_name << std::endl;
    if (db_name.empty()) {
        db_name = ctx.db_default_name;
    }
    if (db_name.empty()) {
        std::clog << "Database name is not provided." << std::endl;
        return false;
    }
    auto db = cnx.get_database(db_name);
    if (!db) {
        std::cerr << "Database " << db_name << " is not found." << std::endl;
        return true;
    }

    auto ts = db->get_time_series(ts_name);
    if (!ts) {
        std::cerr << "Timeseries " << ts_name << " is not found." << std::endl;
        return true;
    }

    auto rec = ts->get_record();
    if (position.state() == parser::helpers::position::INDEX) {
        rec->index(position.index());
    } else if (position.state() == parser::helpers::position::TIME) {
        rec->time(position.time());
    }

    for (size_t n = 0; n < colnames.size(); ++n) {
        rec->set(colnames[n], values[n]);
    }

    if(position.state() == parser::helpers::position::INDEX) {
        ts->insert_record(position.index(), *rec);
    } else if(position.state() == parser::helpers::position::TIME) {
        ts->insert_record(position.time(), *rec);
    }

    return true;
}


} // namespace cyclic::client::commands



bool command_executor::parse_and_execute(const std::string& database, const std::string& command)
{
    commands::command* cmd;

    _context.db_default_name = database;

    std::string::const_iterator begin = command.cbegin(), end = command.cend();

    parser::query_parser<std::string::const_iterator> grammar;

    if(phrase_parse(begin, end, grammar, boost::spirit::ascii::space, cmd))
    {
        return execute(cmd);
    }
    else if(begin != end)
    {
        std::cerr << "Not all requested expression have been parsed:" << std::endl;
        std::cerr << command << std::endl;

        for(auto it = command.cbegin(); it != begin; ++it)
            std::cerr << '-';

        std::cerr << "|" << std::endl;
    }

    return false;
}

bool command_executor::execute(commands::command* command)
{
    return command->execute(_context, *_connection);
}

} // namespace cyclic::client
