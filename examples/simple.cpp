#include "disco/benchmark.h"

#include <vector>
#include <cstdlib>

int main()
{
    disco::init();

    // A benchmark suite that does only time measurement
    disco::suite<> s;

    s.add("rand", []()
          {
              rand();
          });

    s.add("vector push_back", []()
          {
              std::vector<int> v;
              v.push_back(1000);
          });

    // Redirection of each test result to the "console" printer
    s.set_printer<disco::printers::console>();

    // Run all benchmarks
    s.run();

    return 0;
}
