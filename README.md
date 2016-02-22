disco
=====
A micro benchmark library in C++ that supports hardware performance counters.


Examples
--------

### Time measurement
The following code...

```c++
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
```

... will output:

```
  Test              Time (ns)
  ---------------------------
  rand                     14
  vector push_back         47
```

---

### Hardware counters
```c++
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
```

... will output:

```
Test              Time (ns) PAPI_TOT_INS PAPI_TOT_CYC  PAPI_BR_MSP
------------------------------------------------------------------
rand                     14   3530981306   1887375567      2245340
vector push_back         46   4642911637   1889165095         9845
```

