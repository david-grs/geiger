find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(unit_tests basic.cpp timed_run.cpp)
target_link_libraries(unit_tests ${GTEST_BOTH_LIBRARIES} papi pthread)
add_test(unit_tests unit_tests)
