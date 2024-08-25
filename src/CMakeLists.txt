

add_library(cyclicstore SHARED
        common-type.hpp
        common-base.hpp
        common-base.cpp
        common-file.hpp
        common-file.cpp
        libstore.hpp
        libstore.cpp
        libstore-base-impl.hpp
        libstore-base-impl.cpp
        libstore-mem-impl.hpp
        libstore-mem-impl.cpp
        libstore-file-impl.hpp
        libstore-file-impl.cpp
    )

#target_link_libraries(cyclicstore Boost::program_options)
install(TARGETS cyclicstore LIBRARY DESTINATION lib)

install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/common-type.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common-base.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libstore.hpp
        DESTINATION include/cyclicdb
        )


configure_file(cyclicstore.pc.in ${CMAKE_CURRENT_BINARY_DIR}/cyclicstore.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cyclicstore.pc DESTINATION lib/pkgconfig)


add_executable(cyclicstore-cli
        parser-values.hpp
        store-parser-commands.hpp
        store-commands.hpp
        store-commands.cpp
        store.cpp
    )

target_link_libraries(cyclicstore-cli cyclicstore Boost::program_options)

target_compile_definitions(cyclicstore-cli PRIVATE PROJECT_VERSION="${PROJECT_VERSION}")

install(TARGETS cyclicstore-cli DESTINATION bin)

set_target_properties(cyclicstore-cli PROPERTIES OUTPUT_NAME cyclicstore)



include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS DESCRIPTORS
        PROTO_DESCS cyclicdb.proto
)

add_library(cyclicdb-common SHARED
        ${PROTO_SRCS} ${PROTO_HDRS}
        cyclicdb-proto-serial.cpp
)




add_library(cyclicdbsrv SHARED
        common-type.hpp
        cyclicdb-service.cpp
        cyclicdb-service.hpp
        cyclicdbd-server.cpp
        cyclicdbd-server.hpp
        cyclicdbd-server-http.cpp
        cyclicdbd-server-http.hpp
)
target_link_libraries(cyclicdbsrv cyclicstore cyclicdb-common)



add_executable(cyclicdbd
        cyclicdbd.cpp
)
target_link_libraries(cyclicdbd
        cyclicstore cyclicdbsrv cyclicdb-common
        Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)



add_library(cyclicdb-client SHARED
        common-type.hpp
        common-base.hpp
        common-base.cpp
        cyclicdb-client.hpp
        cyclicdb-client.cpp
        cyclicdb-client-http.hpp
        cyclicdb-client-http.cpp
)
target_link_libraries(cyclicdb-client cyclicdb-common)


add_executable(cyclicdb
        cyclicdb-cli.cpp
        client-commands.cpp
        client-commands.hpp
)
target_link_libraries(cyclicdb
        cyclicdb-client cyclicdb-common
        Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)

