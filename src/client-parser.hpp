/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/client-parser.hpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicdb is free software: you can redistribute it and/or
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.;
 */

#ifndef CYCLICDB_CLIENT_PARSER_HPP
#define CYCLICDB_CLIENT_PARSER_HPP

#include "common-base.hpp"

#include "parser-common.hpp"
#include "parser-values.hpp"
#include "parser-helpers.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>

BOOST_FUSION_ADAPT_STRUCT(
        cyclic::field_st,
        (std::string, name)
        (cyclic::data_type, type)
)

/**
 * Parsers for cyclicdb command line tool.
 */
namespace cyclic::client::parser {

namespace helpers = cyclic::parser::helpers;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;




template <typename Iterator>
struct query_parser : qi::grammar<Iterator, commands::command*(), ascii::space_type>
{
    query_parser() : query_parser::base_type(query) {
        using ascii::no_case;
        using ascii::char_;
        using qi::int_;
        using qi::ulong_;
        using qi::long_long;
        using qi::double_;
        using qi::alnum;
        using qi::lexeme;
        using qi::lit;
        using qi::_val;
        using qi::_1;
        using qi::_2;
        using qi::_3;
        using qi::_4;
        using qi::_5;

        database_name   %= lexeme[+alnum];
        timeseries_name %= lexeme[+alnum];

        quoted_string %= lexeme[+alnum] | lexeme['"' >> +(char_ - '"') >> '"'];

        column_name %= quoted_string;
        column_names %= column_name % ',';

        field_desc %= column_name >> type;
        field_descs %= field_desc % ',';

        values %= value % ',';

        pos_index = (-(no_case[lit("index")]) >> ulong_)
                [phoenix::bind(helpers::adapt_position_index, _val, _1)];
        pos_time = (no_case[lit("time")] >> long_long)
                [phoenix::bind(helpers::adapt_position_time, _val, _1)];
        position %= pos_index | pos_time;

        at %= (no_case[lit("at")] >> position);
        opt_at = (-(at))[phoenix::bind(cyclic::parser::helpers::adapt_position_opt, _val, _1)];

        start %= no_case[lit("start")] >> position;
        end   %= no_case[lit("end")] >> position;
        opt_start = (-(start))[phoenix::bind(helpers::adapt_position_opt, _val, _1)];
        opt_end   = (-(end))[phoenix::bind(helpers::adapt_position_opt, _val, _1)];

        timeseries_select = (no_case[lit("select")] >> -(database_name) >> timeseries_name >> (lit("*")|column_names) >> opt_start >> opt_end)
                [_val = phoenix::new_<commands::timeseries_select>(_1, _2, _3, _4, _5)];
        timeseries_insert = (no_case[lit("insert")] >> -(database_name) >> timeseries_name >> -('(' >> column_names >> ')')
                                             >> no_case[lit("values")] >>  '(' >> values >> ')' >> opt_at)
                [_val = phoenix::new_<commands::timeseries_insert>(_1, _2, _3, _4, _5)];
        timeseries_list = (no_case[lit("list")] >> -(database_name) ) [_val = phoenix::new_<commands::timeseries_list>(_1)];
        timeseries_details = (no_case[lit("details")] >> -(database_name) >> timeseries_name ) [_val = phoenix::new_<commands::timeseries_details>(_1, _2)];
        timeseries_create = (no_case[lit("create")] >> -(database_name) >> timeseries_name >>
                                           '(' >> field_descs >> ')' >> no_case[lit("capacity")] >> ulong_
                ) [_val = phoenix::new_<commands::timeseries_create>(_1, _2, _3, _4)];

        timeseries %= ( (no_case[lit("timeseries")] | no_case[lit("ts")]) >> ( timeseries_select | timeseries_insert | timeseries_list | timeseries_details | timeseries_create ) );

        database_create = (no_case[lit("create")] >> database_name)  [_val = phoenix::new_<commands::database_create>(_1)];
        database_status = (no_case[lit("status")] >> database_name)  [_val = phoenix::new_<commands::database_status>(_1)];
        database_list = (no_case[lit("list")]) [_val = phoenix::new_<commands::database_list>()];

        database %= ( (no_case[lit("database")] | no_case[lit("db")]) >> ( database_list | database_status | database_create ) );

        query %= database | timeseries;
    }

    cyclic::parser::types_ type;

    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
    qi::rule<Iterator, std::string(), ascii::space_type> column_name;

    qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> column_names;

    qi::rule<Iterator, std::string(), ascii::space_type> database_name;
    qi::rule<Iterator, std::string(), ascii::space_type> timeseries_name;

    cyclic::parser::value_parser<Iterator> value;
    qi::rule<Iterator, std::vector<cyclic::value_t>(), ascii::space_type> values;

    qi::rule<Iterator, helpers::position(), ascii::space_type> position;
    qi::rule<Iterator, helpers::position(), ascii::space_type> pos_index;
    qi::rule<Iterator, helpers::position(), ascii::space_type> pos_time;
    qi::rule<Iterator, helpers::position(), ascii::space_type> at;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_at;

    qi::rule<Iterator, helpers::position(), ascii::space_type> start;
    qi::rule<Iterator, helpers::position(), ascii::space_type> end;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_start;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_end;

    qi::rule<Iterator, cyclic::field_st(), ascii::space_type> field_desc;
    qi::rule<Iterator, std::vector<cyclic::field_st>(), ascii::space_type> field_descs;

    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries;
    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries_select;
    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries_insert;
    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries_list;
    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries_details;
    qi::rule<Iterator, commands::command*(), ascii::space_type> timeseries_create;


    qi::rule<Iterator, commands::command*(), ascii::space_type> database;
    qi::rule<Iterator, commands::command*(), ascii::space_type> database_list;
    qi::rule<Iterator, commands::command*(), ascii::space_type> database_status;
    qi::rule<Iterator, commands::command*(), ascii::space_type> database_create;

    qi::rule<Iterator, commands::command*(), ascii::space_type> query;
};




} // namespace cyclic::client::parser
#endif //CYCLICDB_CLIENT_PARSER_HPP
