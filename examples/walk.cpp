#include "geiger/geiger.h"

extern "C" {
#include <unistd.h>
#include <sys/time.h>
}

#include <cstdlib>
#include <iostream>
#include <algorithm>

static constexpr int size = 1024 * 1024 * 16;
static constexpr int mask = size - 1;
static constexpr int prime = 7919;

auto linear_walk()
{
    std::vector<char> v(size, 'a');
    int pos = 0;
    int sum = 0;

    return [v = std::move(v), &pos, &sum]()
           {
               sum += v[pos];
               pos = ++pos & mask;
           };
}

auto random_walk()
{
    std::vector<char> v(size, 'a');
    int pos = 0;
    int sum = 0;

    return [v = std::move(v), &pos, &sum]()
           {
               sum += v[pos];
               pos = (pos + prime) & mask;
           };
}

void walk()
{
    using namespace geiger;
    suite<cache_profiler> s;

    s.add("linear walk", &linear_walk)
     .add("random walk", &random_walk)
     .set_printer<printer::console<>>()
     .run(size);
}

int main()
{
    geiger::init();
    walk();
    return 0;
}
