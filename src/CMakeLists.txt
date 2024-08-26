

add_library(cyclicstore SHARED
        common/common-type.hpp
        common/common-base.hpp
        common/common-base.cpp
        common/common-file.hpp
        common/common-file.cpp
        store/libstore.hpp
        store/libstore.cpp
        store/libstore-base-impl.hpp
        store/libstore-base-impl.cpp
        store/libstore-mem-impl.hpp
        store/libstore-mem-impl.cpp
        store/libstore-file-impl.hpp
        store/libstore-file-impl.cpp
    )

#target_link_libraries(cyclicstore Boost::program_options)
install(TARGETS cyclicstore LIBRARY DESTINATION lib)

install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/common/common-type.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common/common-base.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/store/libstore.hpp
        DESTINATION include/cyclicdb
        )


configure_file(cyclicstore.pc.in ${CMAKE_CURRENT_BINARY_DIR}/cyclicstore.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cyclicstore.pc DESTINATION lib/pkgconfig)


add_executable(cyclicstore-cli
        common/parser-values.hpp
        store/store-parser-commands.hpp
        store/store-commands.hpp
        store/store-commands.cpp
        store/store.cpp
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
        common/cyclicdb-proto-serial.cpp
)




add_library(cyclicdbsrv SHARED
        common/common-type.hpp
        server/cyclicdb-service.cpp
        server/cyclicdb-service.hpp
        server/cyclicdbd-server.cpp
        server/cyclicdbd-server.hpp
        server/cyclicdbd-server-http.cpp
        server/cyclicdbd-server-http.hpp
)
target_link_libraries(cyclicdbsrv cyclicstore cyclicdb-common)



add_executable(cyclicdbd
        server/cyclicdbd.cpp
)
target_link_libraries(cyclicdbd
        cyclicstore cyclicdbsrv cyclicdb-common
        Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)



add_library(cyclicdb-client SHARED
        common/common-type.hpp
        common/common-base.hpp
        common/common-base.cpp
        store/libstore-base-impl.hpp
        store/libstore-base-impl.cpp
        store/libstore-mem-impl.hpp
        store/libstore-mem-impl.cpp
        client/cyclicdb-client.hpp
        client/cyclicdb-client.cpp
        client/cyclicdb-client-http.hpp
        client/cyclicdb-client-http.cpp
)
target_link_libraries(cyclicdb-client cyclicdb-common)


add_executable(cyclicdb
        client/cyclicdb-cli.cpp
        client/client-commands.cpp
        client/client-commands.hpp
)
target_link_libraries(cyclicdb
        cyclicdb-client cyclicdb-common
        Boost::program_options PkgConfig::Pistache ${Protobuf_LIBRARIES}
)

