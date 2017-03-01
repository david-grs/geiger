#pragma once

#include "papi.h"
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

