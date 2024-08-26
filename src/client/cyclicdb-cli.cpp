/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/client/cyclicdb-cli.cpp
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

#include "cyclicdb-client.hpp"

#include "client-commands.hpp"

#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define CYCLICDB_NAME		"cyclicdb"

#define CYCLICDB_CONNECTION_ENV_NAME "CYCLICDB_CONNECTION"
#define CYCLICDB_DATABASE_ENV_NAME "CYCLICDB_DATABASE"



void display_version()
{
    std::cout
            << CYCLICDB_NAME /*<< " " << PROJECT_VERSION*/ << std::endl
            << "Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>" << std::endl
            << "License LGPLv2.1+: GNU LGPL version 2.1 or later <http://gnu.org/licenses/lgpl-2.1.html>." << std::endl
            << "This is free software: you are free to change and redistribute it." << std::endl
            << "There is NO WARRANTY, to the extent permitted by law." << std::endl
            ;
}

void display_help(po::options_description& options)
{
    std::cout
            << "Usage : " << CYCLICDB_NAME << " [options] <group> <command> [<args>...]" << std::endl
            << "Available commands:" << std::endl
            << "  database list              : list all databases" << std::endl
            << "  database create <dbname>   : create a database" << std::endl
            << "  database status <dbname>   : get details of a database" << std::endl
            << "  timeseries select [<dbname>] <tsname> [*|<field>[,<field>...]] [start <start>] [end <end>]" << std::endl
            << "          : Extract a part of a timeseries." << std::endl
            << "            If '*' is specified, retrieve all fields." << std::endl
            << "            If <start> is not specified, retrieve records from beginning of stored range." << std::endl
            << "            If <end> is not specified, retrieve records to end of stored range." << std::endl
            << "  timeseries insert [<dbname>] <tsname> [(<field>[,<field>...])] values (<value[,<value>...]) [at <index>]" << std::endl
            << "          : Set or append a record at specified index." << std::endl
            << "            If no <field> is specified, retrieve all fields in the table definition order." << std::endl
            << "            If <index> is not provided, append record at index just following the last record." << std::endl
            << "  timeseries list [<dbname>] : list all timeseries in specified database" << std::endl
            << "  timeseries details [<dbname>] <tsname>" << std::endl
            << "          : get details of a timeseries." << std::endl
            << "  timeseries create [<dbname>] <tsname> (<colname> <coltype>[, ...]) capacity <maxrecordcount>" << std::endl
            << "          : Create a timeseries with this specification." << std::endl
            << options << std::endl;
}

//
// Entry point
//

int main(int argc, char** argv)
{
    std::vector<std::string> extras;
    std::string connection;
//    std::string group;
//    std::string command;
    std::string database;

    po::options_description cnx_opt("Connection options");
    cnx_opt.add_options()
            ("connection,c", po::value<std::string>(&connection),
                            "connection descriptor (<host>:<port>)\n"
                            "If not provided, will be looked at " CYCLICDB_CONNECTION_ENV_NAME " environment variable.")
            ("database,d", po::value<std::string>(&database), "database name\n"
                                                               "Can be specified as an option instead of specifying it in the command arguments (where specified as [<dbname>]).\n"
                                                               "If not provided, the value will be looked at " CYCLICDB_DATABASE_ENV_NAME " environment variable.")
            ;

    po::options_description others("Other options");
    others.add_options()
            ("help,h",    "display this help and exit")
            ("version,v", "output version information and exit")
            ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
//            ("group", po::value<std::string>(&group), "command group")
//            ("command", po::value<std::string>(&command), "command")
            ("extra", po::value<std::vector<std::string>>(&extras)->multitoken(), "extra parameters")
            ;

    po::positional_options_description pd;
    pd/*.add("group", 1).add("command", 1)*/.add("extra", -1);

    po::options_description cmdline_options;
    cmdline_options.add(cnx_opt).add(others).add(hidden);

    po::options_description help_options;
    help_options.add(cnx_opt).add(others);

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

/*
    if(group.empty() || command.empty())
    {
        display_help(help_options);
        return -1;
    }
*/

    if(connection.empty()) {
        auto env = std::getenv(CYCLICDB_CONNECTION_ENV_NAME);
        if(env!=nullptr) {
            connection = env;
        }
        if(connection.empty()) {
            std::cerr << "No connection information provided." << std::endl
                << "Please provide --connection argument or " CYCLICDB_CONNECTION_ENV_NAME " environment variable." << std::endl;
            return -2;
        }
    }

    if(database.empty()) {
        auto env = std::getenv(CYCLICDB_DATABASE_ENV_NAME);
        if(env!=nullptr) {
            database = env;
        }
    }

    std::clog << "Connecting to " << connection << std::endl;
    cyclic::client::command_executor exec{cyclic::client::connection::create_connection(connection)};

    std::ostringstream stm;
    for(std::string s : extras)
    {
        stm << ' ' << s;
    }
    std::string extra = stm.str();
    if(!exec.parse_and_execute(/*group, command,*/ database, extra)) {
        display_help(help_options);
        return -1;
    }

    return 0;
}
