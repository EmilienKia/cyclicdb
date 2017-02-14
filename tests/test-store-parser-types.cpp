/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-store-parser-types.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
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
#include <cppunit/extensions/HelperMacros.h>

#include "store-parser-values.hpp"

using namespace cyclicstore;

class TypeParsingTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TypeParsingTest );
    CPPUNIT_TEST( test_parsing_null );
    CPPUNIT_TEST( test_parsing_bool );
    CPPUNIT_TEST( test_parsing_integer );
    CPPUNIT_TEST( test_parsing_float );
  CPPUNIT_TEST_SUITE_END();

public:
  void test_parsing_null();
  void test_parsing_bool();
  void test_parsing_integer();
  void test_parsing_float();

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TypeParsingTest );

void TypeParsingTest::test_parsing_null()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;


    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    std::string source = "null";
    std::string::const_iterator begin = source.cbegin(), end = source.cend();

    CPPUNIT_ASSERT_MESSAGE("Parse 'null' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
    CPPUNIT_ASSERT_MESSAGE("Parse 'null' completely", begin==end );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'null' with correct type", cyclic::CDB_DT_UNSPECIFIED, value.type() );
}

void TypeParsingTest::test_parsing_bool()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "false";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse 'false' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse 'false' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'false' with correct type", cyclic::CDB_DT_BOOLEAN, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'false' with correct value", (bool)false, (bool)value.value<bool>() );
    }

    {
        std::string source = "true";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse 'true' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse 'true' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'true' with correct type", cyclic::CDB_DT_BOOLEAN, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'true' with correct value", (bool)true, (bool)value.value<bool>() );
    }

}

void TypeParsingTest::test_parsing_integer()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "0";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '0' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '0' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '0' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '0' with correct value", (int64_t)0, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "1";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '1' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '1' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1' with correct value", (int64_t)1, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "-1";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-1' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-1' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1' with correct value", (int64_t)-1, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "4012345678";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '4012345678' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '4012345678' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '4012345678' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '4012345678' with correct value", (int64_t)4012345678l, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "-1234567890";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-1234567890' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-1234567890' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1234567890' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1234567890' with correct value", (int64_t)-1234567890l, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "-1123123123123123123";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-1123123123123123123' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-1123123123123123123' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1123123123123123123' with correct type", cyclic::CDB_DT_SIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1123123123123123123' with correct value", (int64_t)-1123123123123123123ll, (int64_t)value.value<int64_t>() );
    }

    {
        std::string source = "12123123123123123123";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '12123123123123123123' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '12123123123123123123' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '12123123123123123123' with correct type", cyclic::CDB_DT_UNSIGNED_64, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '12123123123123123123' with correct value", (uint64_t)12123123123123123123ull, (uint64_t)value.value<uint64_t>() );
    }
}

void TypeParsingTest::test_parsing_float()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    parser::value_parser<std::string::const_iterator> grammar;
    cyclic::value_t value;

    {
        std::string source = "1.";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '1.' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '1.' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1.' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1.' with correct value", (double)1.0, (double)value.value<double>() );
    }

    {
        std::string source = ".2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '.2' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '.2' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '.2' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '.2' with correct value", (double)0.2, (double)value.value<double>() );
    }

    {
        std::string source = "1.2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '1.2' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '1.2' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1.2' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '1.2' with correct value", (double)1.2, (double)value.value<double>() );
    }

    {
        std::string source = "-1.2";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-1.2' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-1.2' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1.2' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1.2' with correct value", (double)-1.2, (double)value.value<double>() );
    }

    {
        std::string source = "4e6";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '4e6' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '4e6' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '4e6' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '4e6' with correct value", (double)4e6, (double)value.value<double>() );
    }

    {
        std::string source = "-1.2e-3";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-1.2e-3' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-1.2e-3' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1.2e-3' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-1.2e-3' with correct value", (double)-1.2e-3, (double)value.value<double>() );
    }

    {
        std::string source = "-NaN";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse '-NaN' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse '-NaN' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse '-NaN' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_MESSAGE("Parse '-NaN' as not-a-number", std::isnan((double)value.value<double>()) );
    }

    {
        std::string source = "InFiniTy";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'InFiniTy' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' as infinity", std::isinf((double)value.value<double>()) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'InFiniTy' with correct value", (double)std::numeric_limits<double>::infinity(), (double)value.value<double>() );
    }

    {
        std::string source = "-inf";
        std::string::const_iterator begin = source.cbegin(), end = source.cend();

        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' successfully", phrase_parse(begin, end, grammar, boost::spirit::ascii::space, value) );
        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' completely", begin==end );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'InFiniTy' with correct type", cyclic::CDB_DT_FLOAT_8, value.type() );
        CPPUNIT_ASSERT_MESSAGE("Parse 'InFiniTy' as infinity", std::isinf((double)value.value<double>()) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Parse 'InFiniTy' with correct value", (double)-std::numeric_limits<double>::infinity(), (double)value.value<double>() );
    }
}

