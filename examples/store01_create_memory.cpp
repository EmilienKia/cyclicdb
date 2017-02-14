/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * examples/store01_create_memory.cpp
 *
 * cyclicdb/examples are distributed in public domain under the CC0 dedication.
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include "libstore.hpp"

#include <iostream>

int main()
{
    std::unique_ptr<cyclic::table> table = cyclic::store::memory::create(
        {{"first", cyclic::CDB_DT_SIGNED_32},
        {"second", cyclic::CDB_DT_FLOAT_4}},
        60);

    cyclic::raw_record rec;
    rec[0] = 42;
    rec[1] = 12.3;
    table->append_record(rec);

    cyclic::raw_record rec2 = cyclic::raw_record::raw({1, 3.5,});
    table->insert_record((cyclic::record_index_t)3, rec2);

    for(const cyclic::record& rec : *table)
    {
        std::cout << rec[0].value_or<int32_t>(0) << " - " << rec[std::string("second")].value_or<float>(-1.0) << std::endl;
    }

    return 0;
}