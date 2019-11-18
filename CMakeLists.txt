# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.0)
project(mei C)


set(LICENSE BSD)
set(MEI_VERSION_MAJOR 1)
set(MEI_VERSION_MINOR 2)
set(MEI_VERSION_PATCH 0)
set(MEI_VERSION_STRING ${MEI_VERSION_MAJOR}.${MEI_VERSION_MINOR}.${MEI_VERSION_PATCH})

include(CheckSymbolExists)
check_symbol_exists(IOCTL_MEI_NOTIFY_GET linux/mei.h HAVE_GLOBAL_MEI_H)

if(NOT HAVE_GLOBAL_MEI_H)
include_directories(BEFORE "include")
endif(NOT HAVE_GLOBAL_MEI_H)

set(MEI_SOURCES mei.c)
set(MEI_HEADERS libmei.h)

#Secure compile flags
add_definitions(-Wall -Wextra -Wsign-compare -Wformat-security -Winit-self)
add_definitions(-fstack-protector-all --param ssp-buffer-size=4 -ftrapv)
add_definitions(-D_FORTIFY_SOURCE=2 -O2)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")

add_library(${PROJECT_NAME}_objlib OBJECT ${MEI_SOURCES} ${MEI_HEADERS})
set_target_properties(${PROJECT_NAME}_objlib PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
set_target_properties(${PROJECT_NAME}_shared PROPERTIES PUBLIC_HEADER libmei.h)
set_target_properties(${PROJECT_NAME}_shared PROPERTIES VERSION ${MEI_VERSION_STRING})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES SOVERSION ${MEI_VERSION_STRING})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_library(${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
set_target_properties(${PROJECT_NAME}_static PROPERTIES PUBLIC_HEADER libmei.h)
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}_shared ${PROJECT_NAME}_static
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

add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)


# CPack
set(CPACK_PACKAGE_NAME "libmei")
set(CPACK_PACKAGE_VERSION_MAJOR ${MEI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MEI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${MEI_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Intel, Inc")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MEI Interface Library")
set(CPACK_PACKAGE_CONTACT "linux-mei@linux.intel.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/DESCRIPTION)

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${MEI_VERSION_STRING})
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# RPMB Specific
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_LICENSE ${LICENSE})
set(CPACK_RPM_PACKAGE_GROUP "Development")

#Debian Specific
set(CPACK_DEBIAN_PACKAGE_VERSION 1)
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

set(CPACK_GENERATOR "RPM;DEB")

include(CPack)

