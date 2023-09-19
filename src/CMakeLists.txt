
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

target_link_libraries(cyclicstore Boost::program_options)
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
        store-parser-values.hpp
        store-parser-commands.hpp
        store-commands.hpp
        store-commands.cpp
        store.cpp
    )

target_link_libraries(cyclicstore-cli cyclicstore Boost::program_options)

target_compile_definitions(cyclicstore-cli PRIVATE PROJECT_VERSION="${PROJECT_VERSION}")

install(TARGETS cyclicstore-cli DESTINATION bin)

set_target_properties(cyclicstore-cli PROPERTIES OUTPUT_NAME cyclistore)
