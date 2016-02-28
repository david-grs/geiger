function(add_exec_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} geiger)
endfunction(add_example)

add_exec_test(long_run)
