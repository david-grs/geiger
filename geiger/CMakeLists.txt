set(HEADERS geiger.h chrono.h printer.h benchmark.h benchmark.tcc printer_console.h printer_csv.h)

install(DIRECTORY DESTINATION include/geiger)
install(FILES ${HEADERS} DESTINATION include/geiger)

if (USE_PAPI)
install(FILES papi.h DESTINATION include/geiger)
endif()
