/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store-commands.hpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicstore is free software: you can redistribute it and/or
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

#ifndef _CYCLIC_STORE_COMMANDS_HPP_
#define _CYCLIC_STORE_COMMANDS_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common-base.hpp"
#include "libstore.hpp"
#include "libstore-file-impl.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/**
 * cyclicstore command line tool implementation.
 */
namespace cyclicstore
{
/**
 * Helpers for command parsing.
 */
namespace helpers
{

class position
{
public:
    enum STATE
    {
        NONE,
        INDEX,
        TIME
    };

protected:
    STATE _state = NONE;
    cyclic::record_index_t _index = cyclic::record::invalid_index();
    cyclic::record_time_t _time = 0;
public:
    position():_state(NONE){}
    position(const position& pos):_state(pos._state),_index(pos._index),_time(pos._time){}
    position(cyclic::record_index_t index):_state(INDEX),_index(index){}
    position(cyclic::record_time_t time):_state(TIME),_time(time){}
    
    STATE state()const {return _state;}
    cyclic::record_index_t index()const{return _index;}
    cyclic::record_time_t time()const{return _time;}
};
} // namespace helpers

/**
 * Command definitions for cyclicstore tool.
 */
namespace commands
{

class command
{
public:
    virtual ~command() = default;
};

class query : public command
{
public:
    virtual ~query() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table) = 0;
};


class query_with_colnames : public query
{
protected:
    std::vector<std::string> _colnames;
    std::vector<cyclic::field_index_t> _columns;

    query_with_colnames() = default;
    query_with_colnames(const boost::optional<std::vector<std::string>>& colnames);
    query_with_colnames(const std::vector<std::string>& colnames);

    bool deduce_columns(std::shared_ptr<cyclic::store::impl::file_table_impl>);
public:
    virtual ~query_with_colnames() = default;

    const std::vector<std::string>& colnames()const{return _colnames;}
    std::vector<cyclic::field_index_t> colindices()const{return _columns;}
};


class select : public query_with_colnames
{
protected:
    cyclic::record_index_t _start = 0, _end = cyclic::record::absolute_max_index();

    select() = default;

public:
    select(const boost::optional<std::vector<std::string>>& colnames,
        const boost::optional<unsigned long>& start,
        const boost::optional<unsigned long>& end);
    virtual ~select() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;

    cyclic::record_index_t start()const {return _start;}
    cyclic::record_index_t end()const {return _end;}
};


class query_with_colnames_and_position : public query_with_colnames
{
protected:
    helpers::position _position;

    query_with_colnames_and_position() = default;
    query_with_colnames_and_position(const boost::optional<std::vector<std::string>>& colnames,
        const helpers::position& pos);

public:
    virtual ~query_with_colnames_and_position() = default;

    const helpers::position& position()const {return _position;}
};

class query_with_colnames_values_and_position : public query_with_colnames_and_position
{
protected:
    std::vector<cyclic::value_t> _values;

    query_with_colnames_values_and_position() = default;
    query_with_colnames_values_and_position(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos);
public:
    virtual ~query_with_colnames_values_and_position() = default;

    const std::vector<cyclic::value_t>& values()const {return _values;}
};


class insert : public query_with_colnames_values_and_position
{
protected:

    insert() = default;

public:
    insert(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos);

    virtual ~insert() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};

class set : public query_with_colnames_values_and_position
{
protected:
    set() = default;

public:
    set(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos);

    virtual ~set() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};


class append : public query_with_colnames_values_and_position
{
protected:
    append() = default;

public:
    append(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos);

    virtual ~append() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};

class reset : public query_with_colnames_and_position
{
protected:
    reset() = default;

public:
    reset(const boost::optional<std::vector<std::string>>& colnames,
        const helpers::position& pos);

    virtual ~reset() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};



class dump : public select
{
public:
    dump() = default;
    virtual ~dump() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table) override;
};





class status : public query
{
public:
    status() = default;
    virtual ~status() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};





class details : public query
{
public:
    details() = default;
    virtual ~details() = default;
    virtual bool execute(std::shared_ptr<cyclic::store::impl::file_table_impl>) override;
};



class create : public command
{
protected:
    std::vector<cyclic::field_st> fields;
    cyclic::record_index_t capacity;
public:
    create(const std::vector<cyclic::field_st>& fields, cyclic::record_index_t capacity);
    virtual ~create() = default;

    virtual std::shared_ptr<cyclic::store::impl::file_table_impl> execute(const std::string& filename);
};



}; // namespace commands




class command_executor
{
protected:

    std::string filename;

    std::shared_ptr<cyclic::store::impl::file_table_impl> table;


public:
    command_executor(const std::string& filename):filename(filename){}

    bool parse_and_execute(const std::string& command);

    bool execute(commands::command* command);

};



} // namespace cyclicstore
#endif // _CYCLIC_STORE_COMMANDS_HPP_