#pragma once

#include <chrono>
#include <stdexcept>
#include <cmath>

namespace benchmark
{

void init();

namespace detail
{

static inline uint64_t rdtsc()
{
	uint32_t rax, rdx;
	__asm__ __volatile__("rdtsc" : "=a"(rax), "=d"(rdx));
	return ((uint64_t)rdx << 32) + (uint64_t)rax;
}

extern double tsc_freq_ghz;

}

struct time
{
	time() =default;

	void start()
	{
		if (detail::tsc_freq_ghz == .0)
			throw std::runtime_error("benchmark not initialized");

		m_start = detail::rdtsc();
	}

	std::chrono::nanoseconds elapsed() const
	{
		const uint64_t cycles = detail::rdtsc() - m_start;
		return std::chrono::nanoseconds(std::llround(cycles / detail::tsc_freq_ghz));
	}

private:
	uint64_t m_start;
};

}
