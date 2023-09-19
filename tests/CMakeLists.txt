
add_executable(utest
        catch.hpp
        runner.cpp
        test-common-type.cpp
        test-mem-store.cpp
        test-simple-store.cpp
        test-store-parser-types.cpp
        test-store-parser-commands.cpp
        ../src/store-commands.cpp
    )

target_link_libraries(utest cyclicstore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_test(NAME utest COMMAND utest)