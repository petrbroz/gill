if(CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)
    enable_testing()

    include(ExternalProject)
    ExternalProject_Add(
        gtest
        SVN_REPOSITORY http://googletest.googlecode.com/svn/tags/release-1.7.0
        TIMEOUT 10
        INSTALL_COMMAND "" # Disable install step
        UPDATE_COMMAND "" # Disable update step
        LOG_DOWNLOAD ON # Wrap download, configure and build steps in a script to log output
        LOG_CONFIGURE ON
        LOG_BUILD ON
        PREFIX "gtest"
    )

    ExternalProject_Get_Property(gtest source_dir)
    ExternalProject_Get_Property(gtest binary_dir)

    file(GLOB_RECURSE TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
    add_executable(${PROJECT_TEST_TARGET} ${TEST_FILES})
    include_directories(${source_dir}/include)
    add_dependencies(${PROJECT_TEST_TARGET} gtest)
    target_link_libraries(
        ${PROJECT_TEST_TARGET}
        ${PROJECT_LIB_TARGET}
        ${binary_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${binary_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    target_link_libraries(${PROJECT_TEST_TARGET} pthread)
    set_property(TARGET ${PROJECT_TEST_TARGET} PROPERTY CXX_STANDARD 11)
endif()
