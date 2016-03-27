add_library(geiger SHARED printers.cpp)
target_link_libraries(geiger papi)

set(HEADERS benchmark.h benchmark.tcc chrono.h papi.h printers.h)

install(TARGETS geiger DESTINATION lib)
install(DIRECTORY DESTINATION include/geiger)
install(FILES ${HEADERS} DESTINATION include/geiger)
