set(HEADERS benchmark.h benchmark.tcc chrono.h papi.h printers.h)

install(DIRECTORY DESTINATION include/geiger)
install(FILES ${HEADERS} DESTINATION include/geiger)

