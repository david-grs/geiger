#pragma once

#include <chrono>
#include <thread>
#include <stdexcept>
#include <cmath>

namespace geiger
{

namespace detail
{

static inline uint64_t rdtsc()
{
    uint32_t rax, rdx;
    __asm__ __volatile__("rdtsc" : "=a"(rax), "=d"(rdx));
    return ((uint64_t)rdx << 32) + (uint64_t)rax;
}

static inline uint64_t rdtscp()
{
    uint32_t rax, rdx, rcx;
    __asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
    return ((uint64_t)rdx << 32) + (uint64_t)rax;
}

struct tsc
{
    static double& get_freq_ghz()
    {
        static double tsc_freq_ghz = .0;
        return tsc_freq_ghz;
    }
};

}

inline void init()
{
    using clock = std::chrono::high_resolution_clock;

    auto start = clock::now();

    uint64_t rdtsc_start = detail::rdtsc();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    uint64_t rdtsc_end = detail::rdtsc();

    auto end = clock::now();

    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    uint64_t cycles = rdtsc_end - rdtsc_start;

    double& tsc_freq_ghz = detail::tsc::get_freq_ghz();
    tsc_freq_ghz= (double)cycles / duration_ns.count();
}

struct chrono
{
    chrono()
    {
        if (detail::tsc::get_freq_ghz() == .0)
            throw std::runtime_error("benchmark not initialized");
    }

    void start()
    {
        m_start = detail::rdtsc();
    }

    void restart()
    {
        start();
    }

    int64_t elapsed() const
    {
        return detail::rdtsc() - m_start;
    }

    std::chrono::nanoseconds elapsed_time() const
    {
        const int64_t cycles = detail::rdtsc() - m_start;
        return from_cycles(cycles);
    }

    static std::chrono::nanoseconds from_cycles(int64_t cycles)
    {
        return std::chrono::nanoseconds(std::llround(cycles / detail::tsc::get_freq_ghz()));
    }

    template <typename _DurationT>
    static int64_t to_cycles(_DurationT duration)
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() * detail::tsc::get_freq_ghz();
    }

   private:
    uint64_t m_start;
};

}
