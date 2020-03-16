# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2016-2020 Intel Corporation
cmake_minimum_required(VERSION 3.10)
project(mei C)

# Setup version
include(version.cmake)

# Check for new IOCTLs
include(CheckSymbolExists)
check_symbol_exists(IOCTL_MEI_NOTIFY_GET linux/mei.h HAVE_GLOBAL_MEI_H)
if(NOT HAVE_GLOBAL_MEI_H)
    include_directories(BEFORE "include")
endif()

# Library sources
set(MEI_SOURCES mei.c)
set(MEI_HEADERS libmei.h)

# Security linker flags
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")

# Create object library
add_library(${PROJECT_NAME}_objlib OBJECT ${MEI_SOURCES} ${MEI_HEADERS})
set_target_properties(${PROJECT_NAME}_objlib
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Compilation options
target_compile_options(
    ${PROJECT_NAME}_objlib
    PRIVATE -Wall
            -Werror
            -Wextra
            -fstack-protector-all
            --param
            ssp-buffer-size=4
            -ftrapv
            -D_FORTIFY_SOURCE=2
            -O2
            -Wformat
            -Wformat-security
            -Winit-self)

# Shared Library
add_library(${PROJECT_NAME}_shared SHARED
            $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
set_target_properties(
    ${PROJECT_NAME}_shared
    PROPERTIES PUBLIC_HEADER libmei.h VERSION ${MEI_VERSION_STRING}
               SOVERSION ${MEI_VERSION_STRING} OUTPUT_NAME ${PROJECT_NAME})

# Static Library
add_library(${PROJECT_NAME}_static STATIC
            $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
set_target_properties(
    ${PROJECT_NAME}_static PROPERTIES PUBLIC_HEADER libmei.h OUTPUT_NAME
                                                             ${PROJECT_NAME})

# Install Libraries
install(
    TARGETS ${PROJECT_NAME}_shared ${PROJECT_NAME}_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(INPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
    add_custom_target(
        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM)
endif()

# CPack
set(CPACK_PACKAGE_NAME "libmei")
set(CPACK_PACKAGE_VERSION_MAJOR ${MEI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MEI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${MEI_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Intel, Inc")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MEI Interface Library")
set(CPACK_PACKAGE_CONTACT "linux-mei@linux.intel.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/DESCRIPTION)

# Source Package
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${MEI_VERSION_STRING})
add_custom_target(
    dist
    COMMAND ${CMAKE_MAKE_PROGRAM} package_source
    COMMENT "Create the source package")

# RPMB Specific
set(LICENSE BSD)
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_LICENSE ${LICENSE})
set(CPACK_RPM_PACKAGE_GROUP "Development")
find_program(RPM_EXE rpmbuild)
if(RPM_EXE)
    list(APPEND CPACK_GENERATOR "RPM")
endif()

# Debian Specific
set(CPACK_DEBIAN_PACKAGE_VERSION 1)
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
find_program(DEB_EXE debuild)
if(DEB_EXE)
    list(APPEND CPACK_GENERATOR "DEB")
endif()

include(CPack)
