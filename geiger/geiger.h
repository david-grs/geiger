#pragma once

#ifdef USE_PAPI
#include "papi.h"
#endif

#include "chrono.h"
#include "printer.h"
#include "benchmark.h"
#include "printer_console.h"
#include "printer_csv.h"

namespace geiger
{

inline void init()
{
    chrono::init();
}

}

