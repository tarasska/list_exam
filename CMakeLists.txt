cmake_minimum_required(VERSION 3.12)
project(list)

set(CMAKE_CXX_STANDARD 17)

add_executable(list
        gtest/gtest.h gtest/gtest-all.cc gtest/gtest_main.cc
        list.h list.cpp
        counted.cpp counted.h fault_injection.cpp fault_injection.h list_testing.cpp)

add_executable(list_my list.h list.cpp main.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(list -lpthread)