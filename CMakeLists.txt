cmake_minimum_required(VERSION 3.1)

project(risc-v CXX)

include(cmake/third-party.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#------
# Test
#------

get_third_party(gtest)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include)

enable_testing()


#------
# RISC-V
#------

add_subdirectory(risc-v)
