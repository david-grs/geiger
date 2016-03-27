function(add_example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} papi)
endfunction(add_example)

add_example(simple)
add_example(hardware_counters)
add_example(walk)
add_example(full)
