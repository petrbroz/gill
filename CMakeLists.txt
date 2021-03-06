cmake_minimum_required(VERSION 3.0)

project(gill)
set(VERSION_MAJOR 0 CACHE STRING "Project major version number.")
set(VERSION_MINOR 0 CACHE STRING "Project minor version number.")
set(VERSION_PATCH 1 CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

option(BUILD_TESTS "Build unit tests" ON)
set(SPECTRUM_RES 30)
set(SPECTRUM_MIN 370.0)
set(SPECTRUM_MAX 730.0)
add_definitions(-DSPECTRUM_RES=${SPECTRUM_RES} -DSPECTRUM_MIN=${SPECTRUM_MIN} -DSPECTRUM_MAX=${SPECTRUM_MAX})

set(PROJECT_LIB_TARGET ${PROJECT_NAME})
set(PROJECT_BIN_TARGET ${PROJECT_NAME}-cli)
set(PROJECT_TEST_TARGET ${PROJECT_NAME}-test)
set(CMAKE_CONFIGURATION_TYPES debug release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/cmake")

include_directories("src")

add_subdirectory(src)
add_subdirectory(doc)

# Temporarily disabled (googletest was moved to github)
#if (BUILD_TESTS)
#    add_subdirectory(test)
#endif(BUILD_TESTS)
