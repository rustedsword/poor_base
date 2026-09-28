cmake_minimum_required(VERSION 3.21)

if(TARGET poor_base)
    message("Project already exist")
    return()
endif()

project(poor_base
    VERSION 0.1.0
    DESCRIPTION "Header-only library for C with macros for type-generic printing and advanced array operations"
    LANGUAGES C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(poor_base INTERFACE)

target_compile_features(poor_base INTERFACE c_std_23)

target_include_directories(poor_base INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(POOR_BASE_TEST)
    include(CTest)
    add_subdirectory(tests)
endif()

install(TARGETS poor_base EXPORT poor_base-config)

export(TARGETS poor_base
    FILE "${CMAKE_CURRENT_BINARY_DIR}/poor_base-config.cmake"
    )

install(EXPORT poor_base-config
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
    )

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/poor_base-config-version.cmake"
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/poor_base-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
    )

set(pc_includedir "\${prefix}")
cmake_path(APPEND pc_includedir "${CMAKE_INSTALL_INCLUDEDIR}")
file(CONFIGURE OUTPUT poor_base.pc.in CONTENT [[
includedir=@pc_includedir@

Name: @PROJECT_NAME@
Description: @PROJECT_DESCRIPTION@
Version: @PROJECT_VERSION@
Cflags: -I"${includedir}"
]] @ONLY)

# The prefix is added at install time so that cmake --install --prefix is honored
install(CODE [[
    file(READ "${CMAKE_CURRENT_LIST_DIR}/poor_base.pc.in" pc)
    file(WRITE "${CMAKE_CURRENT_LIST_DIR}/poor_base.pc" "prefix=${CMAKE_INSTALL_PREFIX}\n${pc}")
]])

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/poor_base.pc"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/pkgconfig"
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

