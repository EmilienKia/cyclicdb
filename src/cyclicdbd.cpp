/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/cyclicdbd.cpp
 * Copyright (C) 2024 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicdbd is free software: you can redistribute it and/or
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


#include <csignal>

#include "cyclicdbd-server.hpp"
#include "cyclicdbd-server-http.hpp"


using namespace cyclic::proto;

using namespace Pistache;

#define DEFAULT_PORT "9081"


int main() {
    sigset_t signals;
    if (sigemptyset(&signals) != 0
        || sigaddset(&signals, SIGTERM) != 0
        || sigaddset(&signals, SIGINT) != 0
        || sigaddset(&signals, SIGHUP) != 0
        || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0)
    {
        perror("install signal handler failed");
        return 1;
    }

    auto service = cyclic::server::service::load_service("/home/emilien/tests/cyclicdb");
    if(!service) {
        std::cerr << "Service cannot be instantiated." << std::endl;
        return -1;
    }

    auto server = std::make_shared<cyclic_server>(service);
    // TODO inject server into http_server

    std::clog << "cyclicdbd - listening on port " DEFAULT_PORT "..."<< std::endl;

    cyclic_http_server http_server{server, "*:" DEFAULT_PORT};
    http_server.init();
    http_server.start();

    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0)
    {
        std::clog << "received signal " << signal << std::endl;
    }
    else
    {
        std::clog << "sigwait returns " << status << std::endl;
    }

    http_server.shutdown();
}
