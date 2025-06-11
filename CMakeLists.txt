cmake_minimum_required(VERSION 3.10)
project(libfastcsv C)

# Set version
set(LIBFASTCSV_VERSION_MAJOR 0)
set(LIBFASTCSV_VERSION_MINOR 1)
set(LIBFASTCSV_VERSION_PATCH 0)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build test programs" ON)

# Set C standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    csv_config.c
    csv_reader.c
    csv_writer.c
)

# Header files
set(HEADERS
    csv_config.h
    csv_reader.h
    csv_writer.h
)

# Create library
add_library(fastcsv ${SOURCES})

# Include directories
target_include_directories(fastcsv
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Set properties
set_target_properties(fastcsv PROPERTIES
    VERSION ${LIBFASTCSV_VERSION_MAJOR}.${LIBFASTCSV_VERSION_MINOR}.${LIBFASTCSV_VERSION_PATCH}
    SOVERSION ${LIBFASTCSV_VERSION_MAJOR}
    PUBLIC_HEADER "${HEADERS}"
)

# Installation
include(GNUInstallDirs)
install(TARGETS fastcsv
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fastcsv
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fastcsvConfigVersion.cmake"
    VERSION ${LIBFASTCSV_VERSION_MAJOR}.${LIBFASTCSV_VERSION_MINOR}.${LIBFASTCSV_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/fastcsvConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/fastcsv
) 