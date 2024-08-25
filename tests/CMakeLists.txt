
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src ${CMAKE_CURRENT_BINARY_DIR}/../src)

add_executable(utest
        catch.hpp
        runner.cpp
        test-common-type.cpp
        test-mem-store.cpp
        test-simple-store.cpp
        test-parser-types.cpp
        test-store-parser-commands.cpp
        ../src/store-commands.cpp
        test-server.cpp
)

target_link_libraries(utest cyclicstore cyclicdbsrv cyclicdb-client cyclicdb-common Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES})

add_test(NAME utest COMMAND utest)



add_executable(test_http_server test_http_server.cpp)
target_link_libraries(test_http_server
        cyclicstore cyclicdbsrv cyclicdb-client cyclicdb-common Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)


add_executable(test_pistache test_pistache.cpp)
target_link_libraries(test_pistache
        cyclicstore cyclicdbsrv cyclicdb-client cyclicdb-common Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)
