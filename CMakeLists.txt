cmake_minimum_required(VERSION 3.5)

project(misc LANGUAGES C)
include(GNUInstallDirs)

add_library(misc INTERFACE)

target_include_directories(misc INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS misc EXPORT misc-config)

export(TARGETS misc
    FILE "${CMAKE_CURRENT_BINARY_DIR}/misc-config.cmake"
    )

install(EXPORT misc-config
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

