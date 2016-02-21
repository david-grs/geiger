disco
=====
A micro benchmark library in C++ that supports hardware performance counters.

Example
-------
```c++
    using namespace disco;
    suite<instr_profiler, cache_profiler> s;

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
```



Output
------
```
Test              Time (ns) PAPI_TOT_INS PAPI_TOT_CYC  PAPI_BR_MSP  PAPI_L1_DCM  PAPI_L2_DCM  PAPI_L3_TCM
---------------------------------------------------------------------------------------------------------
rand                     14   3693733988   1887588226      2347800        25833         8521           22
vector reserve()         65   4985638505   1889663240         9295        33196         7642           44
vector push_back         45   4682052955   1890649046         8526        28448         6182           15
```

