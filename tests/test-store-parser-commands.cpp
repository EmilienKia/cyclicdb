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

#include "store-commands.hpp"
#include "store-parser-commands.hpp"

using namespace cyclicstore;

commands::command* parse_command(const std::string& command);


commands::command* parse_command(const std::string& command)
{
    std::string::const_iterator begin = command.cbegin(), end = command.cend();

    parser::query_parser<std::string::const_iterator> grammar;

    commands::command* cmd = nullptr;
    if(phrase_parse(begin, end, grammar, boost::spirit::ascii::space, cmd))
    {
        return cmd;
    }
    else
    {
        return nullptr;
    }
}


TEST_CASE("Test select 01", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select * start 2 end 25");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    REQUIRE( select!=nullptr ); // Parse 'select' command

    REQUIRE( select->colnames().size()==0 ); // Have no specified column name
    REQUIRE( select->start().state()==helpers::position::INDEX ); // Have a specified start index
    REQUIRE( select->start().index()==2 ); // Have correct start index
    REQUIRE( select->end().state()==helpers::position::INDEX ); // Have a specified end index
    REQUIRE( select->end().index()==25 ); // Have correct end index
}

TEST_CASE("Test select 02", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select titi, toto");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    REQUIRE( select!=nullptr ); // Parse 'select' command

    REQUIRE( select->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( select->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( select->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( select->start().state()==helpers::position::NONE ); // Have no start index specified
    REQUIRE( select->end().state()==helpers::position::NONE ); // Have no end index specified
}

TEST_CASE("Test select 03", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select * start index 2 end index 25");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    REQUIRE( select!=nullptr ); // Parse 'select' command

    REQUIRE( select->colnames().size()==0 ); // Have no specified column name
    REQUIRE( select->start().state()==helpers::position::INDEX ); // Have a specified start index
    REQUIRE( select->start().index()==2 ); // Have correct start index
    REQUIRE( select->end().state()==helpers::position::INDEX ); // Have a specified end index
    REQUIRE( select->end().index()==25 ); // Have correct end index
}

TEST_CASE("Test select 04", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select * start time 2 end time 25");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    REQUIRE( select!=nullptr ); // Parse 'select' command

    REQUIRE( select->colnames().size()==0 ); // Have no specified column name
    REQUIRE( select->start().state()==helpers::position::TIME ); // Have a specified start time
    REQUIRE( select->start().time()==2 ); // Have correct start time
    REQUIRE( select->end().state()==helpers::position::TIME ); // Have a specified end time
    REQUIRE( select->end().time()==25 ); // Have correct end time
}

TEST_CASE("Test insert 01", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert values ( 12, 34, 56 )");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    REQUIRE( insert!=nullptr ); // Parse 'insert' command

    REQUIRE( insert->colnames().size()==0 ); // Have no specified column names

    REQUIRE( insert->values().size()==3 ); // Have correct number of specified values
    REQUIRE( insert->values()[0].value<int>()==12 ); // Have correct 1st specified value
    REQUIRE( insert->values()[1].value<int>()==34 ); // Have correct 2nd specified value
    REQUIRE( insert->values()[2].value<int>()==56 ); // Have correct 3rd specified value

    REQUIRE( insert->position().state()==helpers::position::NONE ); // Have no specified index
}

TEST_CASE("Test insert 02", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert (titi, toto) values ( 25, 42 ) at 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    REQUIRE( insert!=nullptr ); // Parse 'insert' command

    REQUIRE( insert->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( insert->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( insert->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( insert->values().size()==2 ); // Have correct number of specified values
    REQUIRE( insert->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( insert->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( insert->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( insert->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test insert 03", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert (titi, toto) values ( 25, 42 ) at index 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    REQUIRE( insert!=nullptr ); // Parse 'insert' command

    REQUIRE( insert->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( insert->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( insert->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( insert->values().size()==2 ); // Have correct number of specified values
    REQUIRE( insert->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( insert->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( insert->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( insert->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test insert 04", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert (titi, toto) values ( 25, 42 ) at time 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    REQUIRE( insert!=nullptr ); // Parse 'insert' command

    REQUIRE( insert->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( insert->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( insert->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( insert->values().size()==2 ); // Have correct number of specified values
    REQUIRE( insert->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( insert->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( insert->position().state()==helpers::position::TIME ); // Have a specified time
    REQUIRE( insert->position().time()==12 ); // Have correct specified time
}

TEST_CASE("Test set 01", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set values ( 12, 34, 56 )");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    REQUIRE( set!=nullptr ); // Parse 'set' command

    REQUIRE( set->colnames().size()==0 ); // Have no specified column names

    REQUIRE( set->values().size()==3 ); // Have correct number of specified values
    REQUIRE( set->values()[0].value<int>()==12 ); // Have correct 1st specified value
    REQUIRE( set->values()[1].value<int>()==34 ); // Have correct 2nd specified value
    REQUIRE( set->values()[2].value<int>()==56 ); // Have correct 3rd specified value

    REQUIRE( set->position().state()==helpers::position::NONE ); // Have no specified index
}

TEST_CASE("Test set 02", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set (titi, toto) values ( 25, 42 ) at 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    REQUIRE( set!=nullptr ); // Parse 'set' command

    REQUIRE( set->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( set->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( set->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( set->values().size()==2 ); // Have correct number of specified values
    REQUIRE( set->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( set->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( set->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( set->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test set 03", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set (titi, toto) values ( 25, 42 ) at index 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    REQUIRE( set!=nullptr ); // Parse 'set' command

    REQUIRE( set->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( set->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( set->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( set->values().size()==2 ); // Have correct number of specified values
    REQUIRE( set->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( set->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( set->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( set->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test set 04", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set (titi, toto) values ( 25, 42 ) at time 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    REQUIRE( set!=nullptr ); // Parse 'set' command

    REQUIRE( set->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( set->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( set->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( set->values().size()==2 ); // Have correct number of specified values
    REQUIRE( set->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( set->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( set->position().state()==helpers::position::TIME ); // Have a specified time
    REQUIRE( set->position().time()==12 ); // Have correct specified time
}

TEST_CASE("Test append 01", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append values ( 12, 34, 56 )");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    REQUIRE( append!=nullptr ); // Parse 'append' command

    REQUIRE( append->colnames().size()==0 ); // Have no specified column names

    REQUIRE( append->values().size()==3 ); // Have correct number of specified values
    REQUIRE( append->values()[0].value<int>()==12 ); // Have correct 1st specified value
    REQUIRE( append->values()[1].value<int>()==34 ); // Have correct 2nd specified value
    REQUIRE( append->values()[2].value<int>()==56 ); // Have correct 3rd specified value

    REQUIRE( append->position().state()==helpers::position::NONE ); // Have no specified index
}

TEST_CASE("Test append 02", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append (titi, toto) values ( 25, 42 ) at 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    REQUIRE( append!=nullptr ); // Parse 'append' command

    REQUIRE( append->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( append->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( append->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( append->values().size()==2 ); // Have correct number of specified values
    REQUIRE( append->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( append->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( append->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( append->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test append 03", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append (titi, toto) values ( 25, 42 ) at index 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    REQUIRE( append!=nullptr ); // Parse 'append' command

    REQUIRE( append->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( append->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( append->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( append->values().size()==2 ); // Have correct number of specified values
    REQUIRE( append->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( append->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( append->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( append->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test append 04", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append (titi, toto) values ( 25, 42 ) at time 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    REQUIRE( append!=nullptr ); // Parse 'append' command

    REQUIRE( append->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( append->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( append->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( append->values().size()==2 ); // Have correct number of specified values
    REQUIRE( append->values()[0].value<int>()==25 ); // Have correct 1st specified value
    REQUIRE( append->values()[1].value<int>()==42 ); // Have correct 2nd specified value

    REQUIRE( append->position().state()==helpers::position::TIME ); // Have a specified time
    REQUIRE( append->position().time()==12 ); // Have correct specified time
}

TEST_CASE("Test reset 01", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    REQUIRE( reset!=nullptr ); // Parse 'reset' command

    REQUIRE( reset->colnames().size()==0 ); // Have no specified column names

    REQUIRE( reset->position().state()==helpers::position::NONE ); // Have no specified index
}

TEST_CASE("Test reset 02", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset (titi, toto) at 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    REQUIRE( reset!=nullptr ); // Parse 'reset' command

    REQUIRE( reset->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( reset->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( reset->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( reset->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( reset->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test reset 03", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset (titi, toto) at index 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    REQUIRE( reset!=nullptr ); // Parse 'reset' command

    REQUIRE( reset->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( reset->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( reset->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( reset->position().state()==helpers::position::INDEX ); // Have a specified index
    REQUIRE( reset->position().index()==12 ); // Have correct specified index
}

TEST_CASE("Test reset 04", "[command]")
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset (titi, toto) at time 12");
    REQUIRE( cmd!=nullptr ); // Parse command

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    REQUIRE( reset!=nullptr ); // Parse 'reset' command

    REQUIRE( reset->colnames().size()==2 ); // Have 2 specified column names
    REQUIRE( reset->colnames()[0]=="titi" ); // Have correct 1st column name
    REQUIRE( reset->colnames()[1]=="toto" ); // Have correct 2nd column name

    REQUIRE( reset->position().state()==helpers::position::TIME ); // Have a specified time
    REQUIRE( reset->position().time()==12 ); // Have correct specified time
}
