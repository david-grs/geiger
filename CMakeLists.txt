cmake_minimum_required(VERSION 2.8)

add_definitions(-O3 -ggdb -stdlib=libstdc++ -std=c++1z)
include_directories(.)

add_subdirectory(disco)
add_subdirectory(examples)

