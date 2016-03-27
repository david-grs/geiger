#include "chrono.h"

#include <chrono>
#include <thread>

namespace geiger
{

namespace detail
{

double tsc_freq_ghz = .0;

}

void init()
{
    using clock = std::chrono::high_resolution_clock;

    auto start = clock::now();

    uint64_t rdtsc_start = detail::rdtsc();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    uint64_t rdtsc_end = detail::rdtsc();

    auto end = clock::now();

    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    uint64_t cycles = rdtsc_end - rdtsc_start;
    detail::tsc_freq_ghz = (double)cycles / duration_ns.count();
}

}
