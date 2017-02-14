/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/test-mem-store.cpp
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

#include <limits>
#include <cmath>

#include "libstore.hpp"
#include "common-file.hpp"

class MemoryStoreTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MemoryStoreTest);
        CPPUNIT_TEST(test_memory_store);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_memory_store();
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MemoryStoreTest);


void MemoryStoreTest::test_memory_store()
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

    size_t capacity = 10;

    std::unique_ptr<cyclic::table> table = cyclic::store::memory::create(fields, capacity);
    CPPUNIT_ASSERT_MESSAGE("Db memory have been created", table);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has correct capacity", (cyclic::record_index_t)capacity, (cyclic::record_index_t)table->record_capacity());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table is empty (count)", (cyclic::record_index_t)0, (cyclic::record_index_t)table->record_count());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table is empty (min index)", (cyclic::record_index_t)cyclic::record::invalid_index(), (cyclic::record_index_t)table->min_index());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table is empty (max index)", (cyclic::record_index_t)cyclic::record::invalid_index(), (cyclic::record_index_t)table->max_index());

    table->append_record((cyclic::record_index_t)0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has one rec", (cyclic::record_index_t)1, (cyclic::record_index_t)table->record_count());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has one rec at index 0 (min index)", (cyclic::record_index_t)0, (cyclic::record_index_t)table->min_index());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has one rec at index 0 (max index)", (cyclic::record_index_t)0, (cyclic::record_index_t)table->max_index());


    table->append_record((cyclic::record_index_t)2);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has 3 rec", (cyclic::record_index_t)3, (cyclic::record_index_t)table->record_count());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has min index at 0", (cyclic::record_index_t)0, (cyclic::record_index_t)table->min_index());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has max index at 2", (cyclic::record_index_t)2, (cyclic::record_index_t)table->max_index());

    table->append_record();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has 4 rec", (cyclic::record_index_t)4, (cyclic::record_index_t)table->record_count());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has min index at 0", (cyclic::record_index_t)0, (cyclic::record_index_t)table->min_index());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has max index at 2", (cyclic::record_index_t)3, (cyclic::record_index_t)table->max_index());

    const cyclic::field& field = table->field("5int32");
    CPPUNIT_ASSERT_MESSAGE("Table has field named '5int32'", &field != nullptr);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Field '5int32' is at index", (cyclic::field_index_t)5, field.index());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Field '5int32' has a correct name", std::string{"5int32"}, field.name());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Field '5int32' has a correct type", cyclic::CDB_DT_SIGNED_32, field.type());


    {
        cyclic::record_index_t index = 1;

        auto rec = table->get_record();
        (*rec)[0] = true;
        (*rec)[1] = (int8_t) -8;
        (*rec)[2] = (uint8_t) 8;
        (*rec)[3] = (int16_t) -16;
        (*rec)[4] = (uint16_t) 16;
        table->set_record(index, *rec);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has 4 rec (no record appended)", (cyclic::record_index_t)4, (cyclic::record_index_t)table->record_count());

        auto rec2 = table->get_record(index);
        CPPUNIT_ASSERT_MESSAGE("Table has record at index 1", rec2);

        CPPUNIT_ASSERT_MESSAGE("Record at idx 1 has value at field 0", rec2->has(0));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 1 has value at field '0bool'", rec2->has("0bool"));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 1 field 0 value", rec2->get(0).value<bool>() == true);

        CPPUNIT_ASSERT_MESSAGE("Record at idx 1 has value at field 3", rec2->has(3));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 1 has value at field '3int16'", rec2->has("3int16"));
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Record at idx 1 field 3 value", (int16_t)-16, rec2->get(3).value<int16_t>());
    }

    {
        cyclic::record_index_t index = 5;

        cyclic::raw_record r = cyclic::raw_record::raw({false, -1, 2, -3, nullptr, cyclic::null, 6});
        table->append_record(index, r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Table has 6 records", (cyclic::record_index_t)6, (cyclic::record_index_t)table->record_count());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max index record is 5", (cyclic::record_index_t)index, (cyclic::record_index_t)table->max_index());

        auto rec = table->get_record(index);
        CPPUNIT_ASSERT_MESSAGE("Table has record at index 5", rec);

        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field 1", rec->has(1));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field '1int8'", rec->has("1int8"));
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Record at idx 5 field 1 value", (int8_t)-1, rec->get(1).value<int8_t>());

        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field 4", !rec->has(4));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field '4uint16'", !rec->has("4uint16"));

        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field 5", !rec->has(5));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field '5int32'", !rec->has("5int32"));

        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field 6", rec->has(1));
        CPPUNIT_ASSERT_MESSAGE("Record at idx 5 has value at field '6uint32'", rec->has("6uint32"));
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Record at idx 5 field 6 value", (uint32_t)6, rec->get(6).value<uint32_t>());
    }
}
