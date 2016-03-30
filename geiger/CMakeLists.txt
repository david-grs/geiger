set(HEADERS papi.h chrono.h printer.h benchmark.h benchmark.tcc printer_console.h)

install(DIRECTORY DESTINATION include/geiger)
install(FILES ${HEADERS} DESTINATION include/geiger)
