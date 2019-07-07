/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-common-types.cpp
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

#include "common-type.hpp"

TEST_CASE("Unspecified type", "[types]") {

    using namespace cyclic;
    value_t val;

    REQUIRE( val.type() == CDB_DT_VOID );
    REQUIRE( !val.has_value() );
}

TEST_CASE("Boolean type", "[types]") {

    using namespace cyclic;
    value_t val{false};

    REQUIRE( val.type() == CDB_DT_BOOLEAN );
    REQUIRE( val.value<bool>() == false );

    val = true;

    REQUIRE( val.type() == CDB_DT_BOOLEAN );
    REQUIRE( val.value<bool>() == true );
}

TEST_CASE("Signed 8-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_SIGNED_8;
    typedef int8_t valtype;

    valtype def = -4;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Unsigned 8-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_UNSIGNED_8;
    typedef uint8_t valtype;

    valtype def = 42;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Signed 16-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_SIGNED_16;
    typedef int16_t valtype;

    valtype def = -12345;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Unsigned 16-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_UNSIGNED_16;
    typedef uint16_t valtype;

    valtype def = 54321;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Signed 32-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_SIGNED_32;
    typedef int32_t valtype;

    valtype def = -1234567890l;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Unsigned 32-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_UNSIGNED_32;
    typedef uint32_t valtype;

    valtype def = 4012345678u;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Signed 64-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_SIGNED_64;
    typedef int64_t valtype;

    valtype def = -1123123123123123123ll;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("Unsigned 64-bits integer type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_UNSIGNED_64;
    typedef uint64_t valtype;

    valtype def = 12123123123123123123ull;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("4-bytes floating type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_FLOAT_4;
    typedef float valtype;

    valtype def = -123.456e-3;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}

TEST_CASE("8-bytes floating type", "[types]") {

    using namespace cyclic;

    data_type datatype = CDB_DT_FLOAT_8;
    typedef double valtype;

    valtype def = -123.456e-3;
    value_t val{def};

    REQUIRE( val.type() == datatype );
    REQUIRE( val.value<valtype>() == def );
}


TEST_CASE("Strict type getting", "[types]") {

    int32_t def = 12345;
    cyclic::value_t val{def};

    REQUIRE( val.value<int32_t>() == def );
    REQUIRE_THROWS_AS( val.value_strict<uint16_t>() , std::bad_variant_access);
}

TEST_CASE("Type casting", "[types]") {

    int32_t def = 12345;
    cyclic::value_t val{def};

    int16_t res = val.value<int16_t>();

    REQUIRE( ((int32_t)res) == def );
}
