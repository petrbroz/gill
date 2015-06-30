cmake_minimum_required(VERSION 3.0)

project(gill)
set(VERSION_MAJOR 0 CACHE STRING "Project major version number.")
set(VERSION_MINOR 0 CACHE STRING "Project minor version number.")
set(VERSION_PATCH 1 CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

set(PROJECT_LIB_TARGET ${PROJECT_NAME})
set(PROJECT_BIN_TARGET ${PROJECT_NAME}-cli)
set(PROJECT_TEST_TARGET ${PROJECT_NAME}-test)
set(CMAKE_CONFIGURATION_TYPES debug release)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/cmake")

include_directories("include/core")
include_directories("include/geometry")

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(test)

set_property(TARGET ${PROJECT_LIB_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_BIN_TARGET} PROPERTY CXX_STANDARD 11)
