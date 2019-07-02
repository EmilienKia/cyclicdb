/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-mem-store.cpp
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


TEST_CASE("Memory storage", "[memory]") {

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

    size_t capacity = 10;

    std::unique_ptr<cyclic::table> table = cyclic::store::memory::create(fields, capacity);

    INFO( "Empty table" );
    REQUIRE( table );
    REQUIRE( table->record_capacity() == capacity );
    REQUIRE( table->record_count() == 0 );
    REQUIRE( table->min_index() == cyclic::record::invalid_index() );
    REQUIRE( table->max_index() == cyclic::record::invalid_index() );

    INFO( "Append one record" );
    table->append_record((cyclic::record_index_t)0);
    REQUIRE( table->record_count() == 1 );
    REQUIRE( table->min_index() == 0 );
    REQUIRE( table->max_index() == 0 );

    INFO( "Append two more records" );
    table->append_record((cyclic::record_index_t)2);
    REQUIRE( table->record_count() == 3 );
    REQUIRE( table->min_index() == 0 );
    REQUIRE( table->max_index() == 2 );

    INFO( "Append another record" );
    table->append_record();
    REQUIRE( table->record_count() == 4 );
    REQUIRE( table->min_index() == 0 );
    REQUIRE( table->max_index() == 3 );

    INFO( "Test 5int32 field" )
    const cyclic::field& field = table->field("5int32");
    REQUIRE( &field != nullptr );
    REQUIRE( field.index() == 5 );
    REQUIRE( field.name() == "5int32" );
    REQUIRE( field.type() == cyclic::CDB_DT_SIGNED_32 );


    {
        INFO( "Test setting a specific already existing record" )
        cyclic::record_index_t index = 1;

        auto rec = table->get_record();
        (*rec)[0] = true;
        (*rec)[1] = (int8_t) -8;
        (*rec)[2] = (uint8_t) 8;
        (*rec)[3] = (int16_t) -16;
        (*rec)[4] = (uint16_t) 16;
        table->set_record(index, *rec);

        REQUIRE( table->record_count() == 4 ); // No record appended

        auto rec2 = table->get_record(index);
        REQUIRE( (bool)rec2 == true ); // Table has record at index 1

        REQUIRE( rec2->has(0) ); // Record at idx 1 has value at field 0
        REQUIRE( rec2->has("0bool") ); // Record at idx 1 has value at field '0bool'
        REQUIRE( rec2->get(0).value<bool>() == true ); // Record at idx 1 field 0 value

        REQUIRE( rec2->has(3) ); // Record at idx 1 has value at field 3
        REQUIRE( rec2->has("3int16") ); // Record at idx 1 has value at field '3int16'
        REQUIRE( rec2->get(3).value<int16_t>() == -16 ); // Record at idx 1 field 3 value
    }

    {
        INFO( "Test append record" )
        cyclic::record_index_t index = 5;

        cyclic::raw_record r = cyclic::raw_record::raw({false, -1, 2, -3, nullptr, cyclic::null, 6});
        table->append_record(index, r);
        REQUIRE( table->record_count() == 6 );
        REQUIRE( (cyclic::record_index_t)table->max_index() == index );

        auto rec = table->get_record(index);
        REQUIRE( (bool)rec == true);

        REQUIRE( rec->has(1) ); // Record at idx 5 has value at field 1
        REQUIRE( rec->has("1int8") ); // Record at idx 5 has value at field '1int8'
        REQUIRE( rec->get(1).value<int8_t>() == -1); // Record at idx 5 field 1 value

        REQUIRE( !rec->has(4) ); // Record at idx 5 has value at field 4
        REQUIRE( !rec->has("4uint16") ); // Record at idx 5 has value at field '4uint16'

        REQUIRE( !rec->has(5) ); // Record at idx 5 has value at field 5
        REQUIRE( !rec->has("5int32") ); // Record at idx 5 has value at field '5int32'

        REQUIRE( rec->has(1) ); // Record at idx 5 has value at field 6
        REQUIRE( rec->has("6uint32") ); // Record at idx 5 has value at field '6uint32'
        REQUIRE( rec->get(6).value<uint32_t>() == 6 ); // Record at idx 5 field 6 value
    }
}
