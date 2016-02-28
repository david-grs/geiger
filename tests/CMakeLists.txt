function(add_exec_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} geiger)
endfunction(add_exec_test)

add_exec_test(long_run)
