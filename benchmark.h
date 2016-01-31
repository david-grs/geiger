#pragma once

#include "papi.h"
#include "chrono.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace benchmark
{

struct report
{
	void print()
	{
	}
};

struct test
{
	void run() const
	{
		using namespace boost::accumulators;
		accumulator_set<int64_t, stats<tag::mean>> acc;

		chrono sampling_chrono;
		sampling_chrono.start();
		int64_t elapsed = 0;

		static std::chrono::milliseconds max_sampling_time(100);

		chrono c;

		for (int i = 0; i < 1e3 && sampling_chrono.elapsed_time() < max_sampling_time; ++i)
		{
			c.start();
			m_callable();
			acc(c.elapsed());
		}

		long iterations = std::chrono::seconds(1) / chrono::from_cycles(mean(acc));

		c.restart();

		for (long i = 0; i < iterations; ++i)
			m_callable();

		std::cout << "Ran " << iterations << " iterations during " << std::chrono::duration_cast<std::chrono::milliseconds>(c.elapsed_time()).count() << " ms" << std::endl;
		std::cout << "Time per task: " << chrono::from_cycles(c.elapsed() / (double)iterations).count() << " ns" << std::endl;
	}

	std::string m_name;
	std::function<void()> m_callable;
};

struct suite
{
	suite& add(const std::string& name,
			   std::function<void()> test)
	{
		m_tests.push_back({name, test});
		return *this;
	}

	suite& on_complete(std::function<void()> f)
	{
		m_on_complete = f;
		return *this;
	}

	suite& run()
	{
		for (const auto& p : m_tests)
			p.run();

		if (m_on_complete)
			m_on_complete();

		return *this;
	}

	template <int... _EventsT>
	suite& run_perf_counters()
	{
		m_papi_wrappers.emplace_back(new papi_wrapper<_EventsT...>());
		return *this;
	}

	std::function<void()> m_on_complete;
	std::vector<test> m_tests;
	std::vector<std::unique_ptr<papi_wrapper_base>> m_papi_wrappers;
};

}
