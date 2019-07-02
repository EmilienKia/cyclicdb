/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-store-parser-types.cpp
 * Copyright (C) 2017-2019 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/tests are free software: you can redistribute them and/or
 * modify them under the terms of the GNU General Public License as published by
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
#include "catch.hpp"

#include "store-parser-values.hpp"

using namespace cyclicstore;


TEST_CASE("Parsing null", "[parser]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    std::string source = "null";
    std::string::const_iterator begin = source.cbegin(), end = source.cend();

    REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse 'null' successfully
    REQUIRE( begin==end ); // Parse 'null' completely
    REQUIRE( value.type() == cyclic::CDB_DT_UNSPECIFIED ); // Parse 'null' with correct type
}

TEST_CASE("Parsing boolean", "[parser]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "false";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse 'false' successfully
        REQUIRE( begin==end ); // Parse 'false' completely
        REQUIRE( value.type() == cyclic::CDB_DT_BOOLEAN ); // Parse 'false' with correct type
        REQUIRE( value.value<bool>() == false ); // Parse 'false' with correct value
    }

    {
        std::string source = "true";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse 'true' successfully
        REQUIRE( begin==end ); // Parse 'true' completely
        REQUIRE( value.type() == cyclic::CDB_DT_BOOLEAN ); // Parse 'true' with correct type
        REQUIRE( value.value<bool>() == true ); // Parse 'true' with correct value
    }

}

TEST_CASE("Parsing integer", "[parser]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "0";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '0' successfully
        REQUIRE( begin==end ); // Parse '0' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '0' with correct type
        REQUIRE( value.value<int64_t>() == 0 ); // Parse '0' with correct value
    }

    {
        std::string source = "1";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '1' successfully
        REQUIRE( begin==end ); // Parse '1' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '1' with correct type
        REQUIRE( value.value<int64_t>() == 1 ); // Parse '1' with correct value
    }

    {
        std::string source = "-1";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-1' successfully
        REQUIRE( begin==end ); // Parse '-1' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '-1' with correct type
        REQUIRE( value.value<int64_t>() == -1 ); // Parse '-1' with correct value
    }

    {
        std::string source = "4012345678";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '4012345678' successfully
        REQUIRE( begin==end ); // Parse '4012345678' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '4012345678' with correct type
        REQUIRE( value.value<int64_t>() == 4012345678l ); // Parse '4012345678' with correct value
    }

    {
        std::string source = "-1234567890";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-1234567890' successfully
        REQUIRE( begin==end ); // Parse '-1234567890' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '-1234567890' with correct type
        REQUIRE( value.value<int64_t>() == -1234567890l ); // Parse '-1234567890' with correct value
    }

    {
        std::string source = "-1123123123123123123";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-1123123123123123123' successfully
        REQUIRE( begin==end ); // Parse '-1123123123123123123' completely
        REQUIRE( value.type() == cyclic::CDB_DT_SIGNED_64 ); // Parse '-1123123123123123123' with correct type
        REQUIRE( value.value<int64_t>() == -1123123123123123123ll ); // Parse '-1123123123123123123' with correct value
    }

    {
        std::string source = "12123123123123123123";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '12123123123123123123' successfully
        REQUIRE( begin==end ); // Parse '12123123123123123123' completely
        REQUIRE( value.type() == cyclic::CDB_DT_UNSIGNED_64 ); // Parse '12123123123123123123' with correct type
        REQUIRE( value.value<uint64_t>() == 12123123123123123123ull ); // Parse '12123123123123123123' with correct value
    }
}

TEST_CASE("Parsing float", "[parser]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "1.";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '1.' successfully
        REQUIRE( begin==end ); // Parse '1.' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '1.' with correct type
        REQUIRE( value.value<double>() == 1.0 ); // Parse '1.' with correct value
    }

    {
        std::string source = ".2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '.2' successfully
        REQUIRE( begin==end ); // Parse '.2' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '.2' with correct type
        REQUIRE( value.value<double>() == 0.2 ); // Parse '.2' with correct value
    }

    {
        std::string source = "1.2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '1.2' successfully
        REQUIRE( begin==end ); // Parse '1.2' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '1.2' with correct type
        REQUIRE( value.value<double>() == 1.2 ); // Parse '1.2' with correct value
    }

    {
        std::string source = "-1.2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-1.2' successfully
        REQUIRE( begin==end ); // Parse '-1.2' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '-1.2' with correct type
        REQUIRE( value.value<double>() == -1.2 ); // Parse '-1.2' with correct value
    }

    {
        std::string source = "4e6";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '4e6' successfully
        REQUIRE( begin==end ); // Parse '4e6' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '4e6' with correct type
        REQUIRE( value.value<double>() == 4e6 ); // Parse '4e6' with correct value
    }

    {
        std::string source = "-1.2e-3";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-1.2e-3' successfully
        REQUIRE( begin==end ); // Parse '-1.2e-3' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '-1.2e-3' with correct type
        REQUIRE( value.value<double>() == -1.2e-3 ); // Parse '-1.2e-3' with correct value
    }

    {
        std::string source = "-NaN";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse '-NaN' successfully
        REQUIRE( begin==end ); // Parse '-NaN' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse '-NaN' with correct type
        REQUIRE( std::isnan(value.value<double>()) ); // Parse '-NaN' as not-a-number
    }

    {
        std::string source = "InFiniTy";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse 'InFiniTy' successfully
        REQUIRE( begin==end ); // Parse 'InFiniTy' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse 'InFiniTy' with correct type
        REQUIRE( std::isinf(value.value<double>()) ); // Parse 'InFiniTy' as infinity
        REQUIRE( value.value<double>() == std::numeric_limits<double>::infinity() ); // Parse 'InFiniTy' with correct value
    }

    {
        std::string source = "-inf";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        REQUIRE( phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) ); // Parse 'InFiniTy' successfully
        REQUIRE( begin==end ); // Parse 'InFiniTy' completely
        REQUIRE( value.type() == cyclic::CDB_DT_FLOAT_8 ); // Parse 'InFiniTy' with correct type
        REQUIRE( std::isinf(value.value<double>()) ); // Parse 'InFiniTy' as infinity
        REQUIRE( value.value<double>() == -std::numeric_limits<double>::infinity() ); // Parse 'InFiniTy' with correct value
    }
}
