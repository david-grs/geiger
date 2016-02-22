#include "disco/benchmark.h"

#include<vector>
#include <cstdlib>

int main()
{
    using namespace disco;
    init();

    // instr_profiler reports the number of instructions, cycles and mispredicted branches
    suite<instr_profiler> s;

    // You can cover the events you want by defining your own PAPI wrapper:
    // using branch_profiler = papi_wrapper<PAPI_BR_TKN, PAPI_BR_NTK, PAPI_BR_MSP, PAPI_BR_PRC>
    // (here, branches taken, not taken, mispredicted, correctly predicted)
    //
    // and then use it:
    // suite<branch_profiler> s;

    s.add("rand",
          []()
          {
              rand();
          })
     .add("vector push_back",
          []()
          {
              std::vector<int> v;
              v.push_back(1000);
          })
     .set_printer<printers::console>()
     .run();

     return 0;
}
