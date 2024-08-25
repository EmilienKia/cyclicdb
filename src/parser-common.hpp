/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/parser-common.hpp
 * Copyright (C) 2017-2024 Emilien Kia <emilien.kia@gmail.com>
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

#ifndef CYCLICDB_PARSER_COMMON_HPP
#define CYCLICDB_PARSER_COMMON_HPP

#include "common-base.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
//#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix.hpp>

namespace cyclic::parser {

namespace qi = boost::spirit::qi;

//
// Grammars and symbols
//

struct types_ : qi::symbols<char, cyclic::data_type>
{
    types_()
    {
        add
                ("b"   , cyclic::CDB_DT_BOOLEAN)
                ("i8"  , cyclic::CDB_DT_SIGNED_8)
                ("u8"  , cyclic::CDB_DT_UNSIGNED_8)
                ("i16" , cyclic::CDB_DT_SIGNED_16)
                ("u16" , cyclic::CDB_DT_UNSIGNED_16)
                ("i32" , cyclic::CDB_DT_SIGNED_32)
                ("u32" , cyclic::CDB_DT_UNSIGNED_32)
                ("i64" , cyclic::CDB_DT_SIGNED_64)
                ("u64" , cyclic::CDB_DT_UNSIGNED_64)
                ("f4"  , cyclic::CDB_DT_FLOAT_4)
                ("f8"  , cyclic::CDB_DT_FLOAT_8)

                ("bool"   , cyclic::CDB_DT_BOOLEAN)
                ("int8"   , cyclic::CDB_DT_SIGNED_8)
                ("uint8"  , cyclic::CDB_DT_UNSIGNED_8)
                ("int16"  , cyclic::CDB_DT_SIGNED_16)
                ("uint16" , cyclic::CDB_DT_UNSIGNED_16)
                ("int"    , cyclic::CDB_DT_SIGNED_32)
                ("int32"  , cyclic::CDB_DT_SIGNED_32)
                ("uint32" , cyclic::CDB_DT_UNSIGNED_32)
                ("int64"  , cyclic::CDB_DT_SIGNED_64)
                ("uint64" , cyclic::CDB_DT_UNSIGNED_64)
                ("float4" , cyclic::CDB_DT_FLOAT_4)
                ("float8" , cyclic::CDB_DT_FLOAT_8)

                ("boolean" , cyclic::CDB_DT_BOOLEAN)
                ("char"    , cyclic::CDB_DT_SIGNED_8)
                ("byte"    , cyclic::CDB_DT_UNSIGNED_8)
                ("short"   , cyclic::CDB_DT_SIGNED_16)
                ("integer" , cyclic::CDB_DT_SIGNED_32)
                ("long"    , cyclic::CDB_DT_SIGNED_64)
                ("float"   , cyclic::CDB_DT_FLOAT_4)
                ("double"  , cyclic::CDB_DT_FLOAT_8)
                ;
    }

};


} // namespace cyclic::parser
#endif //CYCLICDB_PARSER_COMMON_HPP
