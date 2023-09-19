
add_executable(create_memory
        EXCLUDE_FROM_ALL
        store01_create_memory.cpp
    )
target_link_libraries(create_memory cyclicstore)


add_executable(create_file
        EXCLUDE_FROM_ALL
        store02_create_file.cpp
    )
target_link_libraries(create_file cyclicstore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
