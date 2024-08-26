/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common/parser-hellpers.hpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * cyclicdb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the COPYING file at the root of the source distribution for more details.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CYCLICDB_PARSER_HELPERS_HPP
#define CYCLICDB_PARSER_HELPERS_HPP

#include "common-base.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace cyclic::parser::helpers {

class position
{
public:
    enum STATE
    {
        NONE,
        INDEX,
        TIME
    };

protected:
    STATE _state = NONE;
    cyclic::record_index_t _index = cyclic::record::invalid_index();
    cyclic::record_time_t _time = 0;
public:
    position():_state(NONE){}
    position(const position& pos):_state(pos._state),_index(pos._index),_time(pos._time){}
    position(cyclic::record_index_t index):_state(INDEX),_index(index){}
    position(cyclic::record_time_t time):_state(TIME),_time(time){}

    STATE state()const {return _state;}
    cyclic::record_index_t index()const{return _index;}
    cyclic::record_time_t time()const{return _time;}
};




inline void adapt_position_opt(helpers::position& res, boost::optional<helpers::position> opt_pos)
{
    if(opt_pos)
    {
        res = *opt_pos;
    }
    else
    {
        res = helpers::position{};
    }
}

inline void adapt_position_index(helpers::position& res, cyclic::record_index_t index)
{
    res = helpers::position{index};
}

inline void adapt_position_time(helpers::position& res, cyclic::record_time_t time)
{
    res = helpers::position{time};
}


} // namespace cyclic::parser::helpers
#endif //CYCLICDB_PARSER_HELPERS_HPP
