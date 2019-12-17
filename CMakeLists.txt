cmake_minimum_required(VERSION 3.5)

project(misc LANGUAGES C)
include(GNUInstallDirs)


add_library(misc INTERFACE)

target_include_directories(misc INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(IN_TREE_BUILD)
    set_target_properties(libdi2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
else()
    install(TARGETS misc EXPORT misc-config)

    export(TARGETS misc
        FILE "${CMAKE_CURRENT_BINARY_DIR}/misc-config.cmake"
    )

    install(EXPORT misc-config
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
endif()
