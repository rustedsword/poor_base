cmake_minimum_required(VERSION 3.14)

if(TARGET poor_base)
    message("Project already exist")
    return()
endif()

project(poor_base LANGUAGES C)
include(GNUInstallDirs)

add_library(poor_base INTERFACE)

target_include_directories(poor_base INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS poor_base EXPORT poor_base-config)

export(TARGETS poor_base
    FILE "${CMAKE_CURRENT_BINARY_DIR}/poor_base-config.cmake"
    )

install(EXPORT poor_base-config
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

