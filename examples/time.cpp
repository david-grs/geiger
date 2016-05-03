#include "geiger/geiger.h"

#include <vector>
#include <cstdlib>

extern "C"
{
#include <sys/time.h>
#include <time.h>
}

__thread std::array<uint64_t, 1000> arr_tls;
    std::array<uint64_t, 1000> arr, arr2;

void* fuck_my_cache(int count)
{
    std::vector<char> v;
    v.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        v.emplace_back(count * 2);
        count++;
    }

    return nullptr;
 //   return &v;
}

int main(int argc, char **argv)
{
    geiger::init();

    // A benchmark suite that does only time measurement
    geiger::suite<geiger::cache_profiler> s;
    uint64_t res = 0;
    int base = atoi(argv[1]);
    float offset = atof(argv[2]);

    std::vector<uint64_t> v;
    v.reserve(1e6);

    //struct timespec ts;
//clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
//v.emplace_back(ts.tv_sec * 1e9 + ts.tv_nsec);



    for (int i = 0; i < arr.size(); ++i)
    {
        arr[i] = base * i + std::round(offset);
        arr2[i] = base * 0.2 * i + std::round(offset);
        arr_tls[i] = base * i;
    }

    s.add("tls only ", [&]()
          {
              uint64_t sum = 0;
                for (int i = 0; i < arr.size(); ++i)
                    sum += arr_tls[i];
               v.emplace_back(sum);
          });

    s.add("standard ", [&]()
          {
              uint64_t sum = 0;
                for (int i = 0; i < arr.size(); ++i)
                    sum += arr[i];
               v.emplace_back(sum);
          });


    s.add("mix tls & no tls ", [&]()
          {
              uint64_t sum = 0;
                for (int i = 0; i < arr.size(); ++i)
                {
                    sum += arr[i];
                    sum += arr_tls[i];
                }
                v.emplace_back(sum);
          });

    s.add("mix no tls ", [&]()
          {
              uint64_t sum = 0;
                for (int i = 0; i < arr.size(); ++i)
                {
                    sum += arr[i];
                    sum += arr2[i];
                }
                v.emplace_back(sum);
          });

    s.add("rdtsc", [&]()
          {
              res += geiger::detail::rdtsc();
          });

    s.add("rdtsc + mul + add", [&]()
          {
              res += geiger::detail::rdtsc() * offset + base;
          });
/*
    s.add("gettimeofday", [&]()
          {
              struct timeval tv;
              gettimeofday(&tv, nullptr);
              res += tv.tv_sec * 1e9 + tv.tv_usec * 1e3;
          });
*/
    s.add("clock_gettime CLOCK_REALTIME", [&]()
          {
              struct timespec ts;
              clock_gettime(CLOCK_REALTIME, &ts);
              v.emplace_back(ts.tv_sec * 1e9 + ts.tv_nsec);
          });

    s.add("clock_gettime CLOCK_MONOTONIC", [&]()
          {
              struct timespec ts;
              clock_gettime(CLOCK_MONOTONIC, &ts);
              v.emplace_back(ts.tv_sec * 1e9 + ts.tv_nsec);
          });


    s.add("clock_gettime CLOCK_MONOTONIC_RAW", [&]()
          {
              struct timespec ts;
              clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
              v.emplace_back(ts.tv_sec * 1e9 + ts.tv_nsec);
          });

    s.add("clock_gettime CLOCK_REALTIME_COARSE", [&]()
          {
              struct timespec ts;
              clock_gettime(CLOCK_REALTIME_COARSE, &ts);
              v.emplace_back(ts.tv_sec * 1e9 + ts.tv_nsec);
          });


    // Redirection of each test result to the "console" printer
    s.set_printer<geiger::printer::console>();

    // Run all benchmarks
    s.run();

    (void)fuck_my_cache(4 << 20);
    sleep(3);

    s.run(1);

    return 0;
}
