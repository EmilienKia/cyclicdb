/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store-parser-values.hpp
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

#ifndef _CYCLIC_STORE_PARSER_VALUES_HPP_
#define _CYCLIC_STORE_PARSER_VALUES_HPP_

#include "common-type.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>

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


template <typename Iterator>
struct value_parser : qi::grammar<Iterator, cyclic::value_t(), ascii::space_type>
{
    value_parser() : value_parser::base_type(value) {
        using qi::_val;
        using qi::_1;

        value_null    = (ascii::string("null"))     [_val = phoenix::bind([]()->cyclic::value_t{return cyclic::value_t{};})];
        value_false   = (ascii::string("false"))    [_val = phoenix::bind([]()->cyclic::value_t{return cyclic::value_t{false};})];
        value_true    = (ascii::string("true"))     [_val = phoenix::bind([]()->cyclic::value_t{return cyclic::value_t{true};})];

        value_explicit %= value_null | value_false | value_true;

        value_uinteger = (qi::ulong_long)            [_val = phoenix::bind([](uint64_t val)->cyclic::value_t{return cyclic::value_t{val};}, _1)];
        value_integer  = (qi::long_long)             [_val = phoenix::bind([](int64_t val)->cyclic::value_t{return cyclic::value_t{val};}, _1)];
        value_float    = (strict_double)             [_val = phoenix::bind([](double val)->cyclic::value_t{return cyclic::value_t{val};}, _1)];

        value %= value_explicit | value_float | value_integer | value_uinteger;
    }

    qi::real_parser<double, qi::strict_real_policies<double>> strict_double;

    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_null;
    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_false;
    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_true;

    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_explicit;

    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_uinteger;
    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_integer;
    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value_float;

    qi::rule<Iterator, cyclic::value_t(), ascii::space_type> value;
};



}} // namespace cyclicstore::parser
#endif // _CYCLIC_STORE_PARSER_VALUES_HPP_