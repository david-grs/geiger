cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 -std=c++14")

include_directories(.)

add_subdirectory(geiger)
add_subdirectory(examples)
add_subdirectory(tests)

