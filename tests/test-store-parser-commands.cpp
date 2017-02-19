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

#include "store-commands.hpp"
#include "store-parser-commands.hpp"

using namespace cyclicstore;

class CommandParsingTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(CommandParsingTest);
        CPPUNIT_TEST(test_select_01);
        CPPUNIT_TEST(test_select_02);

        CPPUNIT_TEST(test_insert_01);
        CPPUNIT_TEST(test_insert_02);

        CPPUNIT_TEST(test_set_01);
        CPPUNIT_TEST(test_set_02);

        CPPUNIT_TEST(test_append_01);
        CPPUNIT_TEST(test_append_02);

        CPPUNIT_TEST(test_reset_01);
        CPPUNIT_TEST(test_reset_02);

    CPPUNIT_TEST_SUITE_END();

public:
    void test_select_01();
    void test_select_02();

    void test_insert_01();
    void test_insert_02();

    void test_set_01();
    void test_set_02();

    void test_append_01();
    void test_append_02();

    void test_reset_01();
    void test_reset_02();

    static commands::command* parse_command(const std::string& command);
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CommandParsingTest );

commands::command* CommandParsingTest::parse_command(const std::string& command)
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

void CommandParsingTest::test_select_01()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select * start 2 end 25");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'select' command", select!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have no specified column name", select->colnames().size()==0);
    CPPUNIT_ASSERT_MESSAGE("Have correct start index", select->start()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct end index", select->end()==25);
}

void CommandParsingTest::test_select_02()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("select titi, toto");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::select* select = dynamic_cast<commands::select*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'select' command", select!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have 2 specified column names", select->colnames().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st column name", select->colnames()[0]=="titi");
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd column name", select->colnames()[1]=="toto");

    CPPUNIT_ASSERT_MESSAGE("Have correct implicit start index", select->start()==0);
    CPPUNIT_ASSERT_MESSAGE("Have correct implicit end index", select->end()==cyclic::record::absolute_max_index());
}

void CommandParsingTest::test_insert_01()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert values ( 12, 34, 56 )");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'insert' command", insert!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have no specified column names", insert->colnames().size()==0);

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", insert->values().size()==3);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", insert->values()[0].value<int>()==12);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", insert->values()[1].value<int>()==34);
    CPPUNIT_ASSERT_MESSAGE("Have correct 3rd specified value", insert->values()[2].value<int>()==56);

    CPPUNIT_ASSERT_MESSAGE("Have no specified index", insert->position().state()==helpers::position::NONE);
}

void CommandParsingTest::test_insert_02()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("insert (titi, toto) values ( 25, 42 ) at 12");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::insert* insert = dynamic_cast<commands::insert*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'insert' command", insert!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have 2 specified column names", insert->colnames().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st column name", insert->colnames()[0]=="titi");
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd column name", insert->colnames()[1]=="toto");

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", insert->values().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", insert->values()[0].value<int>()==25);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", insert->values()[1].value<int>()==42);

    CPPUNIT_ASSERT_MESSAGE("Have a specified index", insert->position().state()==helpers::position::INDEX);
    CPPUNIT_ASSERT_MESSAGE("Have correct specified index", insert->position().index()==12);
}

void CommandParsingTest::test_set_01()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set values ( 12, 34, 56 )");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'set' command", set!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have no specified column names", set->colnames().size()==0);

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", set->values().size()==3);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", set->values()[0].value<int>()==12);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", set->values()[1].value<int>()==34);
    CPPUNIT_ASSERT_MESSAGE("Have correct 3rd specified value", set->values()[2].value<int>()==56);

    CPPUNIT_ASSERT_MESSAGE("Have no specified index", set->position().state()==helpers::position::NONE);
}

void CommandParsingTest::test_set_02()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("set (titi, toto) values ( 25, 42 ) at 12");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::set* set = dynamic_cast<commands::set*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'set' command", set!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have 2 specified column names", set->colnames().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st column name", set->colnames()[0]=="titi");
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd column name", set->colnames()[1]=="toto");

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", set->values().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", set->values()[0].value<int>()==25);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", set->values()[1].value<int>()==42);

    CPPUNIT_ASSERT_MESSAGE("Have a specified index", set->position().state()==helpers::position::INDEX);
    CPPUNIT_ASSERT_MESSAGE("Have correct specified index", set->position().index()==12);
}


void CommandParsingTest::test_append_01()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append values ( 12, 34, 56 )");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'append' command", append!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have no specified column names", append->colnames().size()==0);

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", append->values().size()==3);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", append->values()[0].value<int>()==12);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", append->values()[1].value<int>()==34);
    CPPUNIT_ASSERT_MESSAGE("Have correct 3rd specified value", append->values()[2].value<int>()==56);

    CPPUNIT_ASSERT_MESSAGE("Have no specified index", append->position().state()==helpers::position::NONE);
}

void CommandParsingTest::test_append_02()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("append (titi, toto) values ( 25, 42 ) at 12");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::append* append = dynamic_cast<commands::append*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'append' command", append!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have 2 specified column names", append->colnames().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st column name", append->colnames()[0]=="titi");
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd column name", append->colnames()[1]=="toto");

    CPPUNIT_ASSERT_MESSAGE("Have correct number of specified values", append->values().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st specified value", append->values()[0].value<int>()==25);
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd specified value", append->values()[1].value<int>()==42);

    CPPUNIT_ASSERT_MESSAGE("Have a specified index", append->position().state()==helpers::position::INDEX);
    CPPUNIT_ASSERT_MESSAGE("Have correct specified index", append->position().index()==12);
}


void CommandParsingTest::test_reset_01()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'reset' command", reset!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have no specified column names", reset->colnames().size()==0);

    CPPUNIT_ASSERT_MESSAGE("Have no specified index", reset->position().state()==helpers::position::NONE);
}

void CommandParsingTest::test_reset_02()
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    commands::command* cmd = parse_command("reset (titi, toto) at 12");
    CPPUNIT_ASSERT_MESSAGE("Parse command", cmd!=nullptr);

    commands::reset* reset = dynamic_cast<commands::reset*>(cmd);
    CPPUNIT_ASSERT_MESSAGE("Parse 'reset' command", reset!=nullptr);

    CPPUNIT_ASSERT_MESSAGE("Have 2 specified column names", reset->colnames().size()==2);
    CPPUNIT_ASSERT_MESSAGE("Have correct 1st column name", reset->colnames()[0]=="titi");
    CPPUNIT_ASSERT_MESSAGE("Have correct 2nd column name", reset->colnames()[1]=="toto");

    CPPUNIT_ASSERT_MESSAGE("Have a specified index", reset->position().state()==helpers::position::INDEX);
    CPPUNIT_ASSERT_MESSAGE("Have correct specified index", reset->position().index()==12);
}
