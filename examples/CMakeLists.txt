function(add_example name)
  add_executable(${name} ${name}.cpp)
  
  if (USE_PAPI)
    target_link_libraries(${name} papi)
  endif()
endfunction(add_example)

add_example(simple)
add_example(simple_csv)

if (USE_PAPI)
  add_example(hardware_counters)
  add_example(walk)
  add_example(full)
endif()
