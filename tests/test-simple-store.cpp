/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-simple-store.cpp
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

#include <limits>
#include <cmath>

#include "libstore.hpp"
#include "common-file.hpp"

std::string filename = "test-simple.cydb";
std::unique_ptr<cyclic::table> createTable();
std::unique_ptr<cyclic::table> openTable();
cyclic::field_index_t getFieldCount();
cyclic::record_index_t getRecordCapacity();
void removeTable();


cyclic::field_index_t getFieldCount()
{
    return 11;
}

cyclic::record_index_t getRecordCapacity()
{
    return 20;
}

std::unique_ptr<cyclic::table> createTable()
{
    std::vector<cyclic::field_st> fields{
        {"0bool", cyclic::CDB_DT_BOOLEAN,},
        {"1int8", cyclic::CDB_DT_SIGNED_8},
        {"2uint8", cyclic::CDB_DT_UNSIGNED_8},
        {"3int16", cyclic::CDB_DT_SIGNED_16},
        {"4uint16", cyclic::CDB_DT_UNSIGNED_16},
        {"5int32", cyclic::CDB_DT_SIGNED_32},
        {"6uint32", cyclic::CDB_DT_UNSIGNED_32},
        {"7int64", cyclic::CDB_DT_SIGNED_64},
        {"8uint64", cyclic::CDB_DT_UNSIGNED_64},
        {"9float", cyclic::CDB_DT_FLOAT_4},
        {"Adouble", cyclic::CDB_DT_FLOAT_8}
    };

    return cyclic::store::file::create(filename, cyclic::store::file::COMPACT, fields, getRecordCapacity());
}

std::unique_ptr<cyclic::table> openTable()
{
    return cyclic::store::file::open(filename);
}

void removeTable()
{
    cyclic::io::file::remove(filename);
}

struct row
{
    bool b;
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;
    float f;
    double d;
};

bool operator==(const row& r1, const row& r2)
{
    if(r1.b != r2.b) return false;
    if(r1.i8 != r2.i8) return false;
    if(r1.u8 != r2.u8) return false;
    if(r1.i16 != r2.i16) return false;
    if(r1.u16 != r2.u16) return false;
    if(r1.i32 != r2.i32) return false;
    if(r1.u32 != r2.u32) return false;
    if(r1.i64 != r2.i64) return false;
    if(r1.u64 != r2.u64) return false;

    if(std::isnan(r1.f) && std::isnan(r2.f)) { /* continue */ }
    else if(std::isnan(r1.f) && !std::isnan(r2.f)) return false;
    else if(std::isnan(r2.f) && !std::isnan(r1.f)) return false;
    else if( r1.f != r2.f ) return false;

    if(std::isnan(r1.d) && std::isnan(r2.d)) { /* continue */ }
    else if(std::isnan(r1.d) && !std::isnan(r2.d)) return false;
    else if(std::isnan(r2.d) && !std::isnan(r1.d)) return false;
    else if( r1.d != r2.d ) return false;

    return true;
}

cyclic::record& operator<<(cyclic::mutable_record& rec, const row& r)
{
    rec.set(0, r.b);
    rec.set(1, r.i8);
    rec.set(2, r.u8);
    rec.set(3, r.i16);
    rec.set(4, r.u16);
    rec.set(5, r.i32);
    rec.set(6, r.u32);
    rec.set(7, r.i64);
    rec.set(8, r.u64);
    rec.set(9, r.f);
    rec.set(10, r.d);
    return rec;
}

row& operator<<(row& r, const cyclic::record& rec)
{
    r.b = rec.get<bool>(0);
    r.i8 = rec.get<int8_t>(1);
    r.u8 = rec.get<uint8_t>(2);
    r.i16 = rec.get<int16_t>(3);
    r.u16 = rec.get<uint16_t>(4);
    r.i32 = rec.get<int32_t>(5);
    r.u32 = rec.get<uint32_t>(6);
    r.i64 = rec.get<int64_t>(7);
    r.u64 = rec.get<uint64_t>(8);
    r.f = rec.get<float>(9);
    r.d = rec.get<double>(10);
    return r;
}


TEST_CASE("Simple storage", "[simple]") {

    static row rows[] =
    {
        {false, 42, 42, 42, 42, 42, 42, 42, 42, 42.0, 42.0},
        {true, -42, 42, -42, 42, -42, 42, -42, 42, -42.0, -42.0},
        {false, -42, 42, -12345, 54321, -1234567890l, 4012345678u,
                -1123123123123123123ll, 12123123123123123123ull, -123.456e-3, -123.456e-3},
        {false, 42, 42, 42, 42, 42, 42, 42, 42, std::numeric_limits<float>::min(), std::numeric_limits<double>::min()},
        {false, 42, 42, 42, 42, 42, 42, 42, 42, std::numeric_limits<float>::max(), std::numeric_limits<double>::max()},
        {false, 42, 42, 42, 42, 42, 42, 42, 42, std::numeric_limits<float>::infinity(), std::numeric_limits<double>::infinity()},
        {false, 42, 42, 42, 42, 42, 42, 42, 42, -std::numeric_limits<float>::infinity(), -std::numeric_limits<double>::infinity()},
        {false, 42, 42, 42, 42, 42, 42, 42, 42, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()}
    };
    static size_t rowcount = sizeof(rows)/sizeof(row);

    {
        auto table = createTable();
        REQUIRE( table ); // Db file have been created

        for(const row& r : rows)
        {
            std::unique_ptr<cyclic::mutable_record> rec = table->get_record();
            (*rec) << r;
            table->append_record(*rec);
        }
    }

    {
        auto table = openTable();
        REQUIRE( table ); // Db file have been opened
        REQUIRE( table->field_count() == getFieldCount() ); // Field count
        REQUIRE( table->record_count() == rowcount); // Record count
        for(cyclic::record_index_t r = 0; r<table->record_count(); ++r)
        {
            auto rec = table->get_record(r);
            row rw;
            rw << *rec;
            REQUIRE( rw == rows[r]); // Row content
        }
    }

    removeTable();
}




TEST_CASE("Another simple storage", "[simple]")
{
    static row rows[] =
    {
/* 0 */ {false, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0},
/* 1 */ {true, -1, 1, -1, 1, -1, 1, -1, 1, -1.0, -1.0},
/* 2 */ {false, -2, 2, -200, 200, -200, 200, -200, 200, -200.0, -200.0},
/* 3 */ {true, 3, 3, -333, 333, -30303, 30303, -333333, 333333, -33333.33, -33333.33},
/* 4 */ {false, -4, 4, -4444, 4444, -4444, 4444, -44444, 444444, -444444.44, -444444.44},
/* 5 */ {true, 55, 55, -5555, 5555, -5555, 5555, -55555, 55555, -555555.55e5, -55555.55e5},
/* 6 */ {false, -66, 66, -6666, 6666, -666666, 666666, -666666, 666666, -66666.66e-6, -66666.66e-6},
/* 7 */ {true, -127, 127, -7777, 7777, -77777, 77777, -777777, 777777, 7777777, 7777777},
/* 8 */ {false, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
/* 9 */ {true, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}
    };
    static size_t rowcount = sizeof(rows)/sizeof(row);

    //
    // Creating a table
    //
    {
        auto table = createTable();

        REQUIRE( table ); // Db file have been created

        size_t acc = 0;
        for(size_t n = 0; n < 25; n++)
        {
            std::unique_ptr<cyclic::mutable_record> rec = table->get_record();
            *rec << rows[n%10];
            table->append_record(*rec);
        }

        {
            std::unique_ptr<cyclic::mutable_record> rec = table->get_record();
            for(size_t r = 0; r < rec->size(); ++r)
            {
                rec->set(r, r);
            }
            table->insert_record((cyclic::record_index_t)30, *rec);
            table->insert_record((cyclic::record_index_t)27, *rec);
        }

        {
            std::unique_ptr<cyclic::mutable_record> rec = table->get_record();
            (*rec)[1] = 42;
            (*rec)[3] = 42;
            table->update_record((cyclic::record_index_t)22, *rec);
            table->update_record((cyclic::record_index_t)26, *rec);
        }
    }

    //
    // Testing the table
    //
    {
        auto table = openTable();
        REQUIRE( table ); // Db file can be opened
        if(table)
        {
            REQUIRE( table->field_count() == getFieldCount() ); // Field count
            REQUIRE( table->record_count() == getRecordCapacity() ); // Record count

            REQUIRE( table->min_index() == 11 ); // Record min index
            REQUIRE( table->max_index() == 30 ); // Record max index

#if 0
/* 11: */ {true, -1, 1, -1, 1, -1, 1, -1, 1, -1.0, -1.0},
/* 12: */ {false, -2, 2, -200, 200, -200, 200, -200, 200, -200.0, -200.0},
/* 13: */ {true, 3, 3, -333, 333, -30303, 30303, -333333, 333333, -33333.33, -33333.33},
/* 14: */ {false, -4, 4, -4444, 4444, -4444, 4444, -44444, 444444, -444444.44, -444444.44},
/* 15: */ {true, 55, 55, -5555, 5555, -5555, 5555, -55555, 55555, -555555.55e5, -55555.55e5},
/* 16: */ {false, -66, 66, -6666, 6666, -666666, 666666, -666666, 666666, -66666.66e-6, -66666.66e-6},
/* 17: */ {true, -127, 127, -7777, 7777, -77777, 77777, -777777, 777777, 7777777, 7777777},
/* 18: */ {false, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
/* 19: */ {true, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
/* 20: */ {false, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0},
/* 21: */ {true, -1, 1, -1, 1, -1, 1, -1, 1, -1.0, -1.0},
/* 22: */ {false, 42, 2, 42, 200, -200, 200, -200, 200, -200.0, -200.0},
/* 23: */ {true, 3, 3, -333, 333, -30303, 30303, -333333, 333333, -33333.33, -33333.33},
/* 24: */ {false, -4, 4, -4444, 4444, -4444, 4444, -44444, 444444, -444444.44, -444444.44},
/* 25: */ {}
/* 26: */ {, 42, , 42}
/* 27: */ {false, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
/* 28: */ {}
/* 29: */ {}
/* 30: */ {false, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
#endif // 0

            // Test record 11
            {
                auto r = table->get_record((cyclic::record_index_t)11);
                REQUIRE( r ); // Has a record at index 11

                cyclic::record& rec = *r;
                REQUIRE( rec.ok()); // Has a record at index 11

                REQUIRE( rec.has(0) ); // Record 11 field 0 is set
                REQUIRE( rec[0].type() == cyclic::CDB_DT_BOOLEAN ); // Record 11 field 0 has a boolean value
                REQUIRE( rec[0].value<bool>() == rows[1].b ); // Record 11 field 0 value

                REQUIRE( rec.has(5) ); // Record 11 field 5 is set
                REQUIRE( rec[5].type() == cyclic::CDB_DT_SIGNED_32 ); // Record 11 field 5 has a int32 value
                REQUIRE( rec[5].value<int32_t>() == rows[1].i32 ); // Record 11 field 5 value

                REQUIRE( rec.has(7) ); // Record 11 field 7 is set
                REQUIRE( rec[7].type() == cyclic::CDB_DT_SIGNED_64 ); // Record 11 field 0 has a int64 value
                REQUIRE( rec[7].value<int64_t>() == rows[1].i64 ); // Record 11 field 7 value
            }

            // Test record 13
            {
                auto r = table->get_record((cyclic::record_index_t)13);
                REQUIRE( r ); // Has a record at index 13

                cyclic::record& rec = *r;
                REQUIRE( (bool)rec); // "Has a record at index 13"

                REQUIRE( rec.has(0)); // Record 13 field 0 is set
                REQUIRE( rec[0].type() == cyclic::CDB_DT_BOOLEAN ); // Record 13 field 0 has a boolean value
                REQUIRE( rec[0].value<bool>() == rows[3].b); // Record 13 field 0 value

                REQUIRE( rec.has(5) ); // Record 13 field 5 is set
                REQUIRE( rec[5].type() == cyclic::CDB_DT_SIGNED_32 ); // Record 13 field 5 has a int32 value
                REQUIRE( rec[5].value<int32_t>() == rows[3].i32 ); // Record 13 field 5 value

                REQUIRE( rec.has(7) ); // Record 13 field 7 is set
                REQUIRE( rec[7].type() == cyclic::CDB_DT_SIGNED_64 ); // Record 13 field 0 has a int64 value
                REQUIRE( rec[7].value<int64_t>() == rows[3].i64 ); // Record 13 field 7 value
            }

            // Test record 20
            {
                auto r = table->get_record((cyclic::record_index_t)20);
                REQUIRE( r ); // Has a record at index 20

                cyclic::record& rec = *r;
                REQUIRE( (bool)rec ); // Has a record at index 20

                REQUIRE( rec.has(0) ); // Record 20 field 0 is set
                REQUIRE( rec[0].type() == cyclic::CDB_DT_BOOLEAN ); // Record 20 field 0 has a boolean value
                REQUIRE( rec[0].value<bool>() == rows[0].b ); // Record 20 field 0 value

                REQUIRE( rec.has(5) ); // Record 20 field 5 is set
                REQUIRE( rec[5].type() == cyclic::CDB_DT_SIGNED_32 ); // Record 20 field 5 has a int32 value
                REQUIRE( rec[5].value<int32_t>() == rows[0].i32 ); // Record 20 field 5 value

                REQUIRE( rec.has(7) ); // Record 20 field 7 is set
                REQUIRE( rec[7].type() == cyclic::CDB_DT_SIGNED_64 ); // Record 20 field 0 has a int64 value
                REQUIRE( rec[7].value<int64_t>() == rows[0].i64 ); // Record 20 field 7 value
            }

            // Test record 26
            {
                auto r = table->get_record((cyclic::record_index_t)26);
                REQUIRE( r ); // Has a record at index 26

                cyclic::record& rec = *r;
                REQUIRE( (bool)rec ); // Has a record at index 26

                REQUIRE( !rec.has(0) ); // Record 26 field 0 is not set

                REQUIRE( rec.has(1) ); // Record 26 field 1 is set
                REQUIRE( rec[1].type() == cyclic::CDB_DT_SIGNED_8 ); // Record 26 field 1 has a int8 value
                REQUIRE( rec[1].value<int8_t>() == (int8_t)42 ); // Record 26 field 1 value

                REQUIRE( !rec.has(2) ); // Record 26 field 2 is not set

                REQUIRE( rec.has(3) ); // Record 26 field 3 is set
                REQUIRE( rec[3].type() == cyclic::CDB_DT_SIGNED_16 ); // Record 26 field 3 has a int16 value
                REQUIRE( rec[3].value<int16_t>() == 42 ); // Record 26 field 3 value

                REQUIRE( !rec.has(4) ); // Record 26 field 4 is not set
                REQUIRE( !rec.has(5) ); // Record 26 field 5 is not set
                REQUIRE( !rec.has(6) ); // Record 26 field 6 is not set
            }

            // Test record 27
            {
                auto r = table->get_record((cyclic::record_index_t)27);
                REQUIRE( r ); // Has a record at index 27

                cyclic::record& rec = *r;
                REQUIRE( (bool)rec ); // Has a record at index 27

                REQUIRE( rec.has(0) ); // Record 27 field 0 is set
                REQUIRE( rec[0].type() == cyclic::CDB_DT_BOOLEAN ); // Record 27 field 0 has a boolean value
                REQUIRE( rec[0].value<bool>() == false ); // Record 27 field 0 value

                REQUIRE( rec.has(1) ); // Record 27 field 1 is set
                REQUIRE( rec[1].type() == cyclic::CDB_DT_SIGNED_8 ); // Record 27 field 1 has a int8 value
                REQUIRE( rec[1].value<int8_t>() == 1 ); // Record 27 field 1 value

                REQUIRE( rec.has(2) ); // Record 27 field 2 is set
                REQUIRE( rec[2].type() == cyclic::CDB_DT_UNSIGNED_8 ); // Record 27 field 2 has a uint8 value
                REQUIRE( rec[2].value<uint8_t>() == 2 ); // Record 27 field 2 value

                REQUIRE( rec.has(3) ); // Record 27 field 3 is set
                REQUIRE( rec[3].type() == cyclic::CDB_DT_SIGNED_16 ); // Record 27 field 3 has a int16 value
                REQUIRE( rec[3].value<int16_t>() == 3 ); // Record 27 field 3 value

                REQUIRE( rec.has(4) ); // Record 27 field 4 is set
                REQUIRE( rec[4].type() == cyclic::CDB_DT_UNSIGNED_16 ); // Record 27 field 4 has a uint16 value
                REQUIRE( rec[4].value<uint16_t>() == 4 ); // Record 27 field 4 value
            }

            // Test record 29
            {
                auto r = table->get_record((cyclic::record_index_t)29);
                REQUIRE( r ); // Has a record at index 29

                cyclic::record& rec = *r;
                REQUIRE( (bool)rec ); // Has a record at index 29

                REQUIRE( !rec.has(0) ); // Record 29 field 0 is not set
                REQUIRE( !rec.has(1) ); // Record 29 field 1 is not set
                REQUIRE( !rec.has(2) ); // Record 29 field 2 is not set
                REQUIRE( !rec.has(3) ); // Record 29 field 3 is not set
                REQUIRE( !rec.has(4) ); // Record 29 field 4 is not set
                REQUIRE( !rec.has(5) ); // Record 29 field 5 is not set
                REQUIRE( !rec.has(6) ); // Record 29 field 6 is not set
                REQUIRE( !rec.has(7) ); // Record 29 field 7 is not set
                REQUIRE( !rec.has(8) ); // Record 29 field 8 is not set
                REQUIRE( !rec.has(9) ); // Record 29 field 9 is not set
                REQUIRE( !rec.has(10) ); // Record 29 field 10 is not set
            }
        }
    }
    removeTable();
}
