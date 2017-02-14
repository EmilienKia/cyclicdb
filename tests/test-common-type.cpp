/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-common-types.cpp
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

#include "common-type.hpp"

class TypeTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TypeTest );
    CPPUNIT_TEST( test_unspec );
    CPPUNIT_TEST( test_boolean );
    CPPUNIT_TEST( test_int8 );
    CPPUNIT_TEST( test_uint8 );
    CPPUNIT_TEST( test_int16 );
    CPPUNIT_TEST( test_uint16 );
    CPPUNIT_TEST( test_int32 );
    CPPUNIT_TEST( test_uint32 );
    CPPUNIT_TEST( test_int64 );
    CPPUNIT_TEST( test_uint64 );
    CPPUNIT_TEST( test_float4 );
    CPPUNIT_TEST( test_float8 );

    CPPUNIT_TEST( test_strict );
    CPPUNIT_TEST( test_cast );
  CPPUNIT_TEST_SUITE_END();

public:
  void test_unspec();
  void test_boolean();
  void test_int8();
  void test_uint8();
  void test_int16();
  void test_uint16();
  void test_int32();
  void test_uint32();
  void test_int64();
  void test_uint64();
  void test_float4();
  void test_float8();
  
  void test_strict();
  void test_cast();
  
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TypeTest );

void TypeTest::test_unspec()
{
    using namespace cyclic;
    
    value_t val;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unspecified type", CDB_DT_UNSPECIFIED, val.type());
    CPPUNIT_ASSERT_MESSAGE("Not has value", !val.has_value());
}

void TypeTest::test_boolean()
{
    using namespace cyclic;
    
    value_t val{false};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Boolean type", CDB_DT_BOOLEAN, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Boolean 'false' value", false, val.value<bool>());
    val = true;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Boolean type", CDB_DT_BOOLEAN, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Boolean 'true' value", true, val.value<bool>());
}

void TypeTest::test_int8()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_SIGNED_8;
    typedef int8_t valtype;
       
    valtype def = -4;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 8 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 8 value", def, val.value<valtype>());
}

void TypeTest::test_uint8()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_UNSIGNED_8;
    typedef uint8_t valtype;
       
    valtype def = 42;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 8 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 8 value", def, val.value<valtype>());
}

void TypeTest::test_int16()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_SIGNED_16;
    typedef int16_t valtype;
       
    valtype def = -12345;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 16 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 16 value", def, val.value<valtype>());
}

void TypeTest::test_uint16()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_UNSIGNED_16;
    typedef uint16_t valtype;
       
    valtype def = 54321;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 16 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 16 value", def, val.value<valtype>());
}

void TypeTest::test_int32()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_SIGNED_32;
    typedef int32_t valtype;
       
    valtype def = -1234567890l;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 32 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 32 value", def, val.value<valtype>());
}

void TypeTest::test_uint32()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_UNSIGNED_32;
    typedef uint32_t valtype;
       
    valtype def = 4012345678u;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 32 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 32 value", def, val.value<valtype>());
}

void TypeTest::test_int64()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_SIGNED_64;
    typedef int64_t valtype;
       
    valtype def = -1123123123123123123ll;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 64 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 64 value", def, val.value<valtype>());
}

void TypeTest::test_uint64()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_UNSIGNED_64;
    typedef uint64_t valtype;
       
    valtype def = 12123123123123123123ull;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 64 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 64 value", def, val.value<valtype>());
}

void TypeTest::test_float4()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_FLOAT_4;
    typedef float valtype;
       
    valtype def = -123.456e-3;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 64 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Integer 64 value", def, val.value<valtype>());
}

void TypeTest::test_float8()
{
    using namespace cyclic;
    
    data_type datatype = CDB_DT_FLOAT_8;
    typedef double valtype;
       
    valtype def = -123.456e-3;
    value_t val{def};
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 64 type", datatype, val.type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unisgned integer 64 value", def, val.value<valtype>());
}

void TypeTest::test_strict()
{
    int32_t def = 12345;
    cyclic::value_t val{def};

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Strict value getter", def, val.value<int32_t>());
    
    CPPUNIT_ASSERT_THROW_MESSAGE("Strict value getter invalid", val.value_strict<uint16_t>(), cyclic::type_exception);    
}

void TypeTest::test_cast()
{
    int32_t def = 12345;
    cyclic::value_t val{def};

    int16_t res = val.value<int16_t>();
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Get value with cast", def, (int32_t)res);
}