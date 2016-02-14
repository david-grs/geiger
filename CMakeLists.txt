cmake_minimum_required(VERSION 2.8)
add_executable(test test.cpp chrono.cpp printers.cpp) 
add_definitions(-O3 -ggdb -stdlib=libstdc++ -std=c++1z)
target_link_libraries(test papi)

