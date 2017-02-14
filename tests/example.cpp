/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/example.cpp
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

class ExampleTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ExampleTest );
    CPPUNIT_TEST( testOne );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testOne();
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ExampleTest );


void ExampleTest::setUp()
{
}


void ExampleTest::tearDown()
{
}


void ExampleTest::testOne()
{
  // Set up
  int i = 1;
  float f = 1.0;

  // Process
  int cast = (int)f;

  // Check
  CPPUNIT_ASSERT_EQUAL( i, cast );
}


