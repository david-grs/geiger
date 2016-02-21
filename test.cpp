#include "papi.h"
#include "chrono.h"
#include "benchmark.h"
#include "printers.h"

extern "C" {
#include <unistd.h>
#include <sys/time.h>
}

#include <cstdlib>
#include <iostream>
#include <algorithm>

static void escape(void* p)
{
    asm volatile("" : : "g"(p) : "memory");
}

static void clobber()
{
    asm volatile("" : : : "memory");
}

void foo()
{
    using namespace benchmark;
    suite<instr_profiler, cache_profiler> s;

    s.add("rand",
          []()
          {
              rand();
          })
        .add("random",
             []()
             {
                 random();
             })
        .add("rdtsc",
             []()
             {
                 benchmark::detail::rdtsc();
             })
        .add("gettimeofday()",
             []()
             {
                 struct timeval tv;
                 gettimeofday(&tv, NULL);
             })
        .add("time",
             []()
             {
                 time(NULL);
             })
        .add("srand",
             []()
             {
                 srand(0);
             })
        .add("srand(time)",
             []()
             {
                 srand(time(NULL));
             })
        .add("vector reserve()",
             []()
             {
                 std::vector<int> v;
                 v.reserve(1e3);
             })
        .add("vector push_back",
             []()
             {
                 std::vector<int> v;
                 v.push_back(1000);
             })
        .set_printer<printers::console>()
        .on_test_complete([](const std::string& name, const test_report& r)
                          {

                          })
        .on_complete([](const suite_report& r)
                     {
                         for (const auto& p : r.tests)
                             std::cout << p.first << ":" << p.second.time_per_task().count() << "ns" << std::endl;
                     })
        .run();
}

int main()
{
    benchmark::init();
    foo();
    return 0;
}
