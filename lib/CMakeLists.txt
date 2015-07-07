find_package(YAML REQUIRED)
include_directories(${LIBYAML_INCLUDE_DIR})

file(GLOB_RECURSE LIB_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
#include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(${PROJECT_LIB_TARGET} STATIC ${LIB_FILES})
target_link_libraries(${PROJECT_LIB_TARGET} ${LIBYAML_LIBRARIES})
