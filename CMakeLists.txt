cmake_minimum_required(VERSION 3.10)
project(taskr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Main executable
add_executable(taskr src/main.cpp)

# Test executable
# BUILD_TESTING variable is created by include(CTest)
# Disable testing: cmake -B build -S . -DBUILD_TESTING=OFF
# Enable testing: cmake -B build -S . -DBUILD_TESTING=ON
if (BUILD_TESTING)
    # Update submodules
    include(cmake/UpdateSubmodules.cmake)
    # CTest - GoogleTest
    include(CTest)
    add_subdirectory(external/googletest EXCLUDE_FROM_ALL)


    add_executable(taskr_tests
        tests/test.cpp
        tests/test_errors.cpp
        tests/test_parser.cpp
        tests/test_util.cpp
    )
    target_link_libraries(taskr_tests gtest gtest_main)

    include(GoogleTest)
    # Finds all the Google tests associated with the executable
    gtest_discover_tests(taskr_tests)
endif()

install(TARGETS taskr DESTINATION bin)
