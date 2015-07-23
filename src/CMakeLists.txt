find_package(YAML REQUIRED)
include_directories(${LIBYAML_INCLUDE_DIR})

# Generate SPDs for CIE and RGB based on the configured spectral resolution
add_executable(resample_cie "${PROJECT_SOURCE_DIR}/tools/resample_cie.cpp" "core/math.cpp")
set_property(TARGET resample_cie PROPERTY CXX_STANDARD 11)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_cie.h
    COMMAND resample_cie ${CMAKE_CURRENT_BINARY_DIR}/_cie.h
    DEPENDS resample_cie
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE LIB_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM LIB_FILES "gill.cpp")
add_library(${PROJECT_LIB_TARGET} STATIC ${LIB_FILES} ${CMAKE_CURRENT_BINARY_DIR}/_cie.h)
target_link_libraries(${PROJECT_LIB_TARGET} ${LIBYAML_LIBRARIES})
set_property(TARGET ${PROJECT_LIB_TARGET} PROPERTY CXX_STANDARD 11)

add_executable(${PROJECT_BIN_TARGET} "gill.cpp")
target_link_libraries(${PROJECT_BIN_TARGET} ${PROJECT_LIB_TARGET})
set_property(TARGET ${PROJECT_BIN_TARGET} PROPERTY CXX_STANDARD 11)
