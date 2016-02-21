#include "chrono.h"

#include <chrono>
#include <thread>

extern "C" {
#include <time.h>
}

namespace benchmark
{

namespace detail
{

double tsc_freq_ghz = .0;
}

std::chrono::nanoseconds operator-(timespec end, timespec start)
{
    return std::chrono::nanoseconds((int64_t)((end.tv_sec - start.tv_sec) * 1e9 + end.tv_nsec - start.tv_nsec));
}

void init()
{
    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    uint64_t rdtsc_start = detail::rdtsc();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    uint64_t rdtsc_end = detail::rdtsc();

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    auto ns = end - start;
    uint64_t cycles = rdtsc_end - rdtsc_start;
    detail::tsc_freq_ghz = (double)cycles / ns.count();
}
}
