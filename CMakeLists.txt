cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

option(USE_PAPI "USE_PAPI" ON)

if (USE_PAPI)
  add_definitions("-DUSE_PAPI")
endif()

include_directories(.)

add_subdirectory(geiger)
add_subdirectory(examples)
add_subdirectory(tests)

