cmake_minimum_required(VERSION 3.10)
project(taskr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(taskr
    src/main.cpp
)

install(TARGETS taskr DESTINATION bin)
