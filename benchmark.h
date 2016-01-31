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

struct test
{
	struct report
	{
		report(long iterations, int64_t cycles)
		: m_iterations(iterations), m_cycles(cycles) {}

		long    iteration_count() const { return m_iterations; }
		int64_t total_cycles() const { return m_cycles; }

		double  cycles_per_task() const { return m_cycles / (double)m_iterations; }
		std::chrono::nanoseconds time_per_task() const { return chrono::from_cycles(cycles_per_task()); }

	private:
		long    m_iterations;
		int64_t m_cycles;
	};

	report run() const
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

		return {iterations, c.elapsed()};
	}

	const std::string& name() const { return m_name; }

	std::string m_name;
	std::function<void()> m_callable;
};

struct suite
{
	typedef std::vector<std::pair<std::string, test::report>> report;

	typedef std::function<void(const std::string&, const test::report&)> test_complete_t;
	typedef std::function<void(const suite::report&)> suite_complete_t;

	suite& add(const std::string& name,
			   std::function<void()> test)
	{
		m_tests.push_back({name, test});
		return *this;
	}

	suite& on_test_complete(test_complete_t f)
	{
		m_on_test_complete = f;
		return *this;
	}

	suite& on_complete(suite_complete_t f)
	{
		m_on_complete = f;
		return *this;
	}

	suite& run()
	{
		report r;

		for (const auto& p : m_tests)
		{
			test::report test_report = p.run();
			r.emplace_back(p.name(), test_report);

			if (m_on_test_complete)
				m_on_test_complete(p.name(), test_report);
		}

		if (m_on_complete)
			m_on_complete(r);

		return *this;
	}

	template <int... _EventsT>
	suite& run_perf_counters()
	{
	//	m_papi_wrappers.emplace_back(new papi_wrapper<_EventsT...>());
		return *this;
	}

	std::vector<test> m_tests;

	test_complete_t m_on_test_complete;
	suite_complete_t m_on_complete;
};

}
