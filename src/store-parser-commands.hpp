/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store-parser-commands.hpp
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.;
 */

#ifndef _CYCLIC_STORE_PARSER_COMMANDS_HPP_
#define _CYCLIC_STORE_PARSER_COMMANDS_HPP_

#include "common-base.hpp"

#include "parser-values.hpp"
#include "store-commands.hpp"

#include "parser-common.hpp"
#include "parser-helpers.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
    cyclic::field_st,
    (std::string, name)
    (cyclic::data_type, type)
)


namespace cyclicstore
{
/**
 * Parsers for cyclicstore command line tool.
 */
namespace parser
{


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

namespace helpers = cyclic::parser::helpers;


template <typename Iterator>
struct query_parser : qi::grammar<Iterator, commands::command*(), ascii::space_type>
{
    query_parser() : query_parser::base_type(query) {
        using qi::int_;
        using qi::ulong_;
        using qi::long_long;
        using qi::double_;
        using ascii::char_;
        using ascii::no_case;

        using qi::alnum;

        using qi::lit;
        using qi::lexeme;

        using qi::_val;
        using qi::_1;
        using qi::_2;
        using qi::_3;

        quoted_string %= lexeme[+alnum] | lexeme['"' >> +(char_ - '"') >> '"'];

        column_name %= quoted_string;
        column_names %= column_name % ',';

        start %= no_case[lit("start")] >> position;
        end   %= no_case[lit("end")] >> position;
        opt_start = (-(start))[phoenix::bind(cyclic::parser::helpers::adapt_position_opt, _val, _1)];
        opt_end   = (-(end))[phoenix::bind(cyclic::parser::helpers::adapt_position_opt, _val, _1)];

        values %= value % ',';

        pos_index = (-(no_case[lit("index")]) >> ulong_)
        [phoenix::bind(cyclic::parser::helpers::adapt_position_index, _val, _1)];
        pos_time = (no_case[lit("time")] >> long_long)
        [phoenix::bind(cyclic::parser::helpers::adapt_position_time, _val, _1)];
        position %= pos_index | pos_time;

        at %= (no_case[lit("at")] >> position);
        opt_at = (-(at))[phoenix::bind(cyclic::parser::helpers::adapt_position_opt, _val, _1)];

        select = (no_case[lit("select")] >> (lit("*")|column_names) >> opt_start >> opt_end )
                [_val = phoenix::new_<commands::select>(_1, _2, _3)];

        dump = no_case[lit("dump")][_val = phoenix::new_<commands::dump>()];
        status = no_case[lit("status")][_val = phoenix::new_<commands::status>()];
        details = no_case[lit("details")][_val = phoenix::new_<commands::details>()];


        field_desc %= column_name >> type;
        field_descs %= field_desc % ',';
        create = (no_case[lit("create")] >> '(' >> field_descs >> ')' >> no_case[lit("capacity")] >> ulong_)
                [_val = phoenix::new_<commands::create>(_1, _2)];

        insert = (    no_case[lit("insert")] >> -('(' >> column_names >> ')')
                   >> no_case[lit("values")] >>  '(' >> values >> ')'
                   >> opt_at
                )[_val = phoenix::new_<commands::insert>(_1, _2, _3)];

        set = (    no_case[lit("set")] >> -('(' >> column_names >> ')')
                   >> no_case[lit("values")] >>  '(' >> values >> ')'
                   >> opt_at
                )[_val = phoenix::new_<commands::set>(_1, _2, _3)];

        append = (    no_case[lit("append")] >> -('(' >> column_names >> ')')
                   >> no_case[lit("values")] >>  '(' >> values >> ')'
                   >> opt_at
                )[_val = phoenix::new_<commands::append>(_1, _2, _3)];

        reset = (    no_case[lit("reset")] >> -('(' >> column_names >> ')')
                   >> opt_at
                )[_val = phoenix::new_<commands::reset>(_1, _2)];

        query %= select | dump | status | details | create | insert | set | append | reset;

    }

    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
    qi::rule<Iterator, std::string(), ascii::space_type> column_name;

    qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> column_names;

    qi::rule<Iterator, helpers::position(), ascii::space_type> start;
    qi::rule<Iterator, helpers::position(), ascii::space_type> end;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_start;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_end;


    qi::rule<Iterator, commands::command*(), ascii::space_type> select;
    qi::rule<Iterator, commands::command*(), ascii::space_type> dump;
    qi::rule<Iterator, commands::command*(), ascii::space_type> status;
    qi::rule<Iterator, commands::command*(), ascii::space_type> details;

    cyclic::parser::types_ type;
    qi::rule<Iterator, cyclic::field_st(), ascii::space_type> field_desc;
    qi::rule<Iterator, std::vector<cyclic::field_st>(), ascii::space_type> field_descs;
    qi::rule<Iterator, commands::command*(), ascii::space_type> create;

    cyclic::parser::value_parser<Iterator> value;
    qi::rule<Iterator, std::vector<cyclic::value_t>(), ascii::space_type> values;

    qi::rule<Iterator, helpers::position(), ascii::space_type> position;
    qi::rule<Iterator, helpers::position(), ascii::space_type> pos_index;
    qi::rule<Iterator, helpers::position(), ascii::space_type> pos_time;
    qi::rule<Iterator, helpers::position(), ascii::space_type> at;
    qi::rule<Iterator, helpers::position(), ascii::space_type> opt_at;

    qi::rule<Iterator, commands::command*(), ascii::space_type> insert;
    qi::rule<Iterator, commands::command*(), ascii::space_type> set;
    qi::rule<Iterator, commands::command*(), ascii::space_type> append;
    qi::rule<Iterator, commands::command*(), ascii::space_type> reset;

    qi::rule<Iterator, commands::command*(), ascii::space_type> query;
};



}} // namespace cyclicstore::parser
#endif // _CYCLIC_STORE_PARSER_COMMANDS_HPP_