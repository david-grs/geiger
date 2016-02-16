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

static inline uint64_t rdtscp()
{
	uint32_t rax, rdx, rcx;
	__asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
	return ((uint64_t)rdx << 32) + (uint64_t)rax;
}

extern double tsc_freq_ghz;

}

struct chrono
{
	chrono()
	{
		if (detail::tsc_freq_ghz == .0)
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
		return std::chrono::nanoseconds(std::llround(cycles / detail::tsc_freq_ghz));
	}

	template <typename _DurationT>
	static int64_t to_cycles(_DurationT duration)
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() * detail::tsc_freq_ghz;
	}

private:
	uint64_t m_start;
};

}
