/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tests/cpputest.cpp
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

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>


int main(int argc, char* argv[])
{
	/* Get the top level suite from the registry */
	CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

	/* Adds the test to the list of test to run */
	CppUnit::TextUi::TestRunner runner;
	runner.addTest( suite );

	/* Change the default outputter to a compiler error format outputter */
	runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
	/* Run the tests. */
	bool wasSucessful = runner.run();

	/* Return error code 1 if the one of test failed. */
	return wasSucessful ? 0 : 1;
}

