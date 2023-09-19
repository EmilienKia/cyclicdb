/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "store-commands.hpp"

#define CYCLICSTORE_NAME		"cyclicstore"

std::shared_ptr<cyclic::store::impl::file_table_impl> table;
std::string filename;


void display_version()
{
    std::cout
        << CYCLICSTORE_NAME << " " << PROJECT_VERSION << std::endl
        << "Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>" << std::endl
        << "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>." << std::endl
        << "This is free software: you are free to change and redistribute it." << std::endl
        << "There is NO WARRANTY, to the extent permitted by law." << std::endl
        ;
}

void display_help(po::options_description& options)
{
    std::cout
        << "Usage : " << CYCLICSTORE_NAME << " [options] <file> [<command> [<args>...]]" << std::endl
        << "Available commands:" << std::endl
        << "  select [*|<field>[,<field>...]] [start <start>] [end <end>]" << std::endl
        << "          : Extract a part of the table." << std::endl
        << "            If '*' is specified, retrieve all fields." << std::endl
        << "            If <start> is not specified, retrieve records from begining of stored range." << std::endl
        << "            If <end> is not specified, retrieve records to end of stored range." << std::endl
        << "  append [(<field>[,<field>...])] values (<value[,<value>...]) [at <index>]" << std::endl
        << "          : Append a record at specified index, the record shall not already exists." << std::endl
        << "            If no <field> is specified, retrieve all fields in the table definition order." << std::endl
        << "            If <index> is not provided, append record at index just following the last record." << std::endl
        << "  set    [(<field>[,<field>...])] values (<value[,<value>...]) [at <index>]" << std::endl
        << "          : Set a record at specified index, the record shall already exists." << std::endl
        << "            If no <field> is specified, retrieve all fields in the table definition order." << std::endl
        << "            If <index> is not provided, set the last record." << std::endl
        << "  insert [(<field>[,<field>...])] values (<value[,<value>...]) [at <index>]" << std::endl
        << "          : Set or append a record at specified index." << std::endl
        << "            If no <field> is specified, retrieve all fields in the table definition order." << std::endl
        << "            If <index> is not provided, append record at index just following the last record." << std::endl
        << "  reset [(<field>[,<field>...])] [at <index>]" << std::endl
        << "          : Reset fields of a record at specified index." << std::endl
        << "            If no <field> is specified, reset all fields in the record." << std::endl
        << "            If <index> is not provided, reset field(s) in the last record." << std::endl
        << "  create (<colname> <coltype>[, ...]) capacity <maxrecordcount>" << std::endl
        << "          : Create a table with this specification." << std::endl
        << "  status  : Display usage status of table." << std::endl
        << "  details : Diplay table details." << std::endl
        << "  dump    : Display all content of table (can be long, depends on table size)." << std::endl
        << options << std::endl;
}

int main(int argc, const char** argv)
{
    std::vector<std::string> extras;


    po::options_description others("Other options");
    others.add_options()
            ("help,h",    "display this help and exit")
            ("version,v", "output version information and exit")
    ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value<std::string>(&filename), "input file")
        ("extra", po::value<std::vector<std::string>>(&extras)->multitoken(), "extra parameters")
    ;

    po::positional_options_description pd;
    pd.add("input-file", 1)/*.add("command", 1)*/.add("extra", -1);

    po::options_description cmdline_options;
    cmdline_options.add(others).add(hidden);

    po::options_description help_options;
    help_options.add(others);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(pd).run(), vm);
    po::notify(vm);


    if(vm.count("version"))
    {
        display_version();
        return 0;
    }

    if(vm.count("help"))
    {
        display_help(help_options);
        return 0;
    }

    if(filename.empty())
    {
        display_help(help_options);
        return -1;
    }

    std::ostringstream stm;
    for(std::string s : extras)
    {
        stm << ' ' << s;
    }

    cyclicstore::command_executor executor(filename);
    return executor.parse_and_execute(stm.str()) ? 0 : -1;
}
