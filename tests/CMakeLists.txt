find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(unit_tests timed_run.cpp)
target_link_libraries(unit_tests ${GTEST_BOTH_LIBRARIES} geiger pthread)
add_test(unit_tests unit_tests)
