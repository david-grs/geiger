set(HEADERS geiger.h papi.h chrono.h printer.h benchmark.h benchmark.tcc printer_console.h printer_csv.h)

install(DIRECTORY DESTINATION include/geiger)
install(FILES ${HEADERS} DESTINATION include/geiger)
