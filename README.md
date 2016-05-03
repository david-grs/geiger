geiger
=====
A micro benchmark library in C++ that supports hardware performance counters.

Why and what ?
 - Because you cannot do a micro-benchmark by running _perf_
 - Simple API, header-only library
 - Each test is run either a number of iterations or a specified time


Build & install
---------------
```bash
$ mkdir build && cd build
$ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
# make install
```


Examples
--------

### Time measurement
The simplest usage of geiger is to measure the time required for a task:

```c++
  #include <geiger/geiger.h>

  #include <vector>
  #include <cstdlib>
  
  int main()
  {
      // This is mandatory before running any benchmarks
      geiger::init();
  
      // A benchmark suite that does only time measurement
      geiger::suite<> s;
  
      s.add("rand", []()
            {
                std::rand();
            });
  
      s.add("vector push_back", []()
            {
                std::vector<int> v;
                v.push_back(1000);
            });
  
      // Redirection of each test result to the "console" printer
      s.set_printer<geiger::printer::console<>>();
  
      // Run each test during one second
      s.run();
  
      return 0;
  }
```

This code will output:

```
  Test              Time (ns)
  ---------------------------
  rand                     14
  vector push_back         47
```

By default - as in the example above - each test is running during one second. Here, the "rand" test has then been executed
tens of thousands of times, and the average execution time was 14ns. 

You can specify the duration you want as argument to *geiger::suite::run()*...

```c++
      // Run each test during one millisecond
      s.run(std::chrono::milliseconds(1));
```

... or a number of iterations:

```c++
      // Run each test exactly 100 times
      s.run(100);
```

Simply be aware that a too short time - or a too low number of iterations - can result in less accurate measurements.

When specifying a duration, before running the benchmark, geiger is performing a calibration stage where it approximates the number of iterations required to run this task during the specified time.


---

### Hardware counters
A more advanced usage of geiger is to include hardware counters. This is done by specifying a list of *papi_wrapper<_EventT...>* in the
template parameters list of *geiger::suite<>*:

```c++
      // cache_profiler reports the number of L1, L2 and L3 cache misses.
      suite<cache_profiler> s;
 
      s.add("linear walk", linear_walk())
       .add("random walk", random_walk());

      s.run();
```

The output is now also displaying the number of hardware events per test run. 

```
Test                Time (ns)      PAPI_L1_DCM      PAPI_L2_DCM      PAPI_L3_TCM
--------------------------------------------------------------------------------
linear walk                88                1                0                0
random walk               613               64               64               55
```

You can cover the events you want by defining your own PAPI wrapper. For example, if you are interested by events
around branch predictions:

```c++
      // Measuring branches taken, not taken, mispredicted and correctly predicted
      using branch_profiler = papi_wrapper<PAPI_BR_TKN, PAPI_BR_NTK, PAPI_BR_MSP, PAPI_BR_PRC>

      suite<branch_profiler> s;
```


---

### Events
### Implementation details
