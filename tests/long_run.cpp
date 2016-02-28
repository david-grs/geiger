#include "geiger/benchmark.h"

#include <vector>
#include <cstdlib>

int main()
{
    geiger::init();

    // A benchmark suite that does only time measurement
    geiger::suite<> s;

    s.add("sleep(0.9)", []()
          {
              timespec tv;
              tv.tv_sec = 0;
              tv.tv_nsec = 9e8;
              nanosleep(&tv, nullptr);
          });

    s.add("sleep(3)", []()
          {
              sleep(3)
          });

    // Redirection of each test result to the "console" printer
    s.set_printer<geiger::printers::console>();

    // Run all benchmarks
    s.run();

    return 0;
}
