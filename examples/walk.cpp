#include "geiger/geiger.h"

extern "C" {
#include <unistd.h>
#include <sys/time.h>
}

#include <cstdlib>
#include <iostream>
#include <algorithm>

static const int size = 1024 * 1024 * 16;
static const int batch = 64;
static const int mask = size - 1;
static const int prime = 7919;
static int sum = 0;

auto linear_walk()
{
    std::vector<char> v(size, 'a');

    return  [v = std::move(v)]()
            {
                static std::size_t pos = std::rand() & mask;

                for (int i = 0; i < batch; ++i)
                {
                    sum += v[pos];
                    pos = (pos + 1) & mask;
                }
            };
}

auto random_walk()
{
    std::vector<char> v(size, 'a');

    return  [v = std::move(v)]()
            {
                static std::size_t pos = std::rand() & mask;

                for (int i = 0; i < batch; ++i)
                {
                    sum += v[pos];
                    pos = (pos * prime) & mask;
                }
            };
}

void walk()
{
    using namespace geiger;
    suite<cache_profiler> s;

    s.add("linear walk", linear_walk())
     .add("random walk", random_walk())
     .set_printer<printer::console<>>()
     .run(size / batch);
}

int main()
{
    geiger::init();
    walk();
    return 0;
}
