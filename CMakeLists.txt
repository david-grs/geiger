cmake_minimum_required(VERSION 2.8)

add_definitions(-O3 -g -stdlib=libstdc++ -std=c++14)
include_directories(.)

add_subdirectory(disco)
add_subdirectory(examples)

