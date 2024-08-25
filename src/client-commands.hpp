/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/client-commands.hpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicdb-cli is free software: you can redistribute it and/or
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


#ifndef CYCLICDB_CLIENT_COMMANDS_HPP
#define CYCLICDB_CLIENT_COMMANDS_HPP

#include "cyclicdb-client.hpp"

#include "common-type.hpp"
#include "parser-common.hpp"
#include "parser-helpers.hpp"

#include <boost/optional.hpp>

namespace cyclic::client {


struct execution_context {
    std::string db_default_name;
};


/**
 * Command definitions for cyclicdb tool.
 */
namespace commands {


//
// Commands
//

class command {
public:
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) = 0;
};


class database_list : public command
{
public:
    database_list() = default;
    virtual ~database_list() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;
};

class database_status : public command
{
protected:
    std::string name;

public:
    database_status(const std::string& name):name(name) {}
    virtual ~database_status() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;
};

class database_create : public command
{
protected:
    std::string name;

public:
    database_create(const std::string& name): name(name) {}
    virtual ~database_create() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;
};

class timeseries_list : public command
{
protected:
    std::string db_name;

public:
    timeseries_list(const boost::optional<std::string>& db_name):db_name(db_name ? *db_name : "") {}
    virtual ~timeseries_list() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;

};

class timeseries_select : public command
{
protected:
    std::string db_name;
    std::string ts_name;
    std::vector<std::string> colnames;
    cyclic::parser::helpers::position start , end;

public:
    timeseries_select(const boost::optional<std::string>& db_name,
                      const std::string & ts_name,
                      const boost::optional<std::vector<std::string>>& colnames,
                      const cyclic::parser::helpers::position& start,
                      const cyclic::parser::helpers::position& end) :
        db_name(db_name ? *db_name : ""), ts_name(ts_name),
        colnames(colnames ? *colnames : std::vector<std::string>{}),
        start(start), end(end) {}
    virtual ~timeseries_select() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;

};

class timeseries_insert : public command
{
protected:
    std::string db_name;
    std::string ts_name;
    std::vector<std::string> colnames;
    std::vector<cyclic::value_t> values;
    cyclic::parser::helpers::position position;

public:
    timeseries_insert(const boost::optional<std::string>& db_name,
                      const std::string & ts_name,
                      const boost::optional<std::vector<std::string>>& colnames,
                      const std::vector<cyclic::value_t>& values,
                      const cyclic::parser::helpers::position& pos):
            db_name(db_name ? *db_name : ""), ts_name(ts_name),
            colnames(colnames ? *colnames : std::vector<std::string>{}),
            values(values), position(pos) {}
    virtual ~timeseries_insert() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;

};

class timeseries_details : public command
{
protected:
    std::string db_name;
    std::string ts_name;

public:
    timeseries_details(const boost::optional<std::string>& db_name, const std::string& ts_name):
        db_name(db_name ? *db_name : ""), ts_name(ts_name) {}
    virtual ~timeseries_details() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;

};

class timeseries_create : public command
{
protected:
    std::string db_name;
    std::string ts_name;
    std::vector<cyclic::field_st> fields;
    cyclic::record_index_t capacity;

public:
    timeseries_create(const boost::optional<std::string>& db_name, const std::string ts_name,
                    const std::vector<cyclic::field_st>& fields, cyclic::record_index_t capacity):
                    db_name(db_name ? *db_name : ""),
                    ts_name(ts_name), fields(fields), capacity(capacity){}
    virtual ~timeseries_create() = default;
    virtual bool execute(const execution_context& ctx, cyclic::client::connection &cnx) override;

};


} // namespace cyclic::client::commands

//
// Command parser and executor
//

class command_executor
{
protected:
    std::shared_ptr<cyclic::client::connection> _connection;

    execution_context _context;

public:
    command_executor(std::shared_ptr<cyclic::client::connection> connection) :
            _connection(std::move(connection)) {}

    bool parse_and_execute(const std::string& database, const std::string& command);

    bool execute(commands::command* command);
};



} // namespace cyclic::client
#endif //CYCLICDB_CLIENT_COMMANDS_HPP
