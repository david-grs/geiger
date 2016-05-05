#include "geiger/benchmark.h"
#include "geiger/printer_console.h"

extern "C" {
    #include <unistd.h>
    #include <sys/time.h>
}

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>

/*
static void escape(void* p)
{
    asm volatile("" : : "g"(p) : "memory");
}

static void clobber()
{
    asm volatile("" : : : "memory");
}
*/

void foo()
{
    using namespace geiger;
    suite<instr_profiler, cache_profiler> s;

    s.add("rand",
          []()
          {
              std::rand();
          })
        .add("random",
             []()
             {
                 random();
             })
        .add("rdtsc",
             []()
             {
                 geiger::detail::rdtsc();
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
                 std::time(NULL);
             })
        .add("srand",
             []()
             {
                 std::srand(0);
             })
        .add("srand(time)",
             []()
             {
                 std::srand(std::time(NULL));
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
        .set_printer<printer::console<>>()
        .on_test_complete([](const std::string&, const test_report&)
                          {

                          })
        .on_complete([](const suite_report&)
                     {
                     //    for (const auto& p : r.tests)
                     //        std::cout << p.first << ":" << p.second.time_per_task().count() << "ns" << std::endl;
                     })
        .run();
}

int main()
{
    geiger::init();
    foo();
    return 0;
}
