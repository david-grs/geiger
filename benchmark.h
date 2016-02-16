#pragma once

#include "papi.h"
#include "chrono.h"
#include "printers.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <tuple>

namespace benchmark
{

struct test_report
{
	test_report(long iterations, int64_t cycles, std::vector<long long>&& papi_counters = {})
	: m_iterations(iterations), m_cycles(cycles), m_papi_counters(papi_counters) {}

	long    iteration_count() const { return m_iterations; }
	int64_t total_cycles() const { return m_cycles; }

	double  cycles_per_task() const { return m_cycles / (double)m_iterations; }
	std::chrono::nanoseconds time_per_task() const { return chrono::from_cycles(cycles_per_task()); }

	const std::vector<long long>& papi_counters() const { return m_papi_counters; }

private:
	long    m_iterations;
	int64_t m_cycles;
	std::vector<long long> m_papi_counters;
};

struct test_base
{
	test_base(const std::string& name)
	 : m_name(name) {}
	virtual ~test_base() {}

	virtual test_report run() const =0;

	const std::string& name() const { return m_name; }

private:
	std::string m_name;
};

template <typename _CallableT, typename... _PAPIWrappersT>
struct test : public test_base
{
	test(const std::string& name, _CallableT&& callable = _CallableT())
	 : test_base(name),
	   m_callable(callable) {}

	test_report run() const override
	{
		std::tuple<_PAPIWrappersT...> papi_wrappers;

		using namespace boost::accumulators;
		accumulator_set<int64_t, stats<tag::mean>> acc;

		chrono sampling_chrono;
		sampling_chrono.start();
		int64_t elapsed = 0;

		constexpr std::chrono::milliseconds max_sampling_time(100);

		chrono c;

		for (int i = 0; i < 1e6 && sampling_chrono.elapsed_time() < max_sampling_time; ++i)
		{
			c.start();
			m_callable();
			acc(c.elapsed());
		}

		long iterations = std::chrono::milliseconds(1) / chrono::from_cycles(mean(acc));

		auto run_benchmark = [&]() -> auto
		{
			c.restart();

			for (long i = 0; i < iterations; ++i)
				m_callable();

			return c.elapsed();
		};

		auto next_iterations_count = [&](int64_t cycles)
		{
			iterations = std::lround(iterations / ((double)cycles / chrono::to_cycles(std::chrono::milliseconds(1))));
		};

		constexpr long papi_wrapppers_count = static_cast<long>(std::tuple_size<decltype(papi_wrappers)>::value);

		int64_t total_cycles = 0;
		long total_iterations = 0;

		if (papi_wrapppers_count == 0)
		{
			for (int i = 0; i < 1e3; ++i)
			{
				int64_t cycles = run_benchmark();

				total_iterations += iterations;
				total_cycles += cycles;
				next_iterations_count(cycles);
			}

			return {total_iterations, total_cycles};
		}

		std::vector<long long> counters;

		boost::fusion::for_each(papi_wrappers, [&](auto& papi)
		{
			counters.resize(counters.size() + papi.get_counters().size());
			int j = counters.size() - papi.get_counters().size();

			for (int i = 0; i < 1e3; ++i)
			{
				papi.start();
				int64_t cycles = run_benchmark();
				papi.stop();

				total_iterations += iterations;
				total_cycles += cycles;
				next_iterations_count(cycles);

				const auto& curr_counters = papi.get_counters();

				for (int i = 0; i < curr_counters.size(); ++i)
					counters[i + j] += curr_counters[i];
			}
		});

		return {total_iterations, total_cycles, std::move(counters)};
	}

private:
	_CallableT m_callable;
};

struct suite_report
{
	std::vector<std::pair<std::string, test_report>> tests;
};

struct suite_base
{
	virtual ~suite_base() {}

	virtual std::vector<std::reference_wrapper<const std::string>> test_names() const =0;
	virtual std::vector<int> papi_events() const =0;

};

template <typename... _PAPIWrappersT>
struct suite : public suite_base
{
	typedef std::function<void(const std::string&, const test_report&)> test_complete_t;
	typedef std::function<void(const suite_report&)> suite_complete_t;

	template <typename _CallableT>
	suite& add(const std::string& name,
			   _CallableT&& callable)
	{
		m_tests.emplace_back(new test<_CallableT, _PAPIWrappersT...>(name, std::move(callable)));
		return *this;
	}

	template <typename _PrinterT>
	suite& set_printer(_PrinterT&& printer = _PrinterT())
	{
		m_printer.reset(new _PrinterT(std::move(printer)));
		return *this;
	}

	std::vector<std::reference_wrapper<const std::string>> test_names() const override
	{
		std::vector<std::reference_wrapper<const std::string>> v;

		for (const auto& test : m_tests)
			v.push_back(test->name());

		return v;
	}

	std::vector<int> papi_events() const override
	{
		std::vector<int> v;
		std::tuple<_PAPIWrappersT...> papi_wrappers;

		boost::fusion::for_each(papi_wrappers, [&v](auto& papi)
		{
			for (int event : papi.get_event_types())
				v.push_back(event);
		});

		return v;
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
		suite_report r;

		if (m_printer)
			m_printer->on_start(*this);

		for (const auto& p : m_tests)
		{
			test_report test_report = p->run();
			r.tests.emplace_back(p->name(), test_report);

			if (m_on_test_complete)
				m_on_test_complete(p->name(), test_report);

			if (m_printer)
				m_printer->on_test_complete(p->name(), test_report);
		}

		if (m_on_complete)
			m_on_complete(r);

		return *this;
	}

	std::vector<std::unique_ptr<test_base>> m_tests;
	std::unique_ptr<printer_base> m_printer;

	test_complete_t m_on_test_complete;
	suite_complete_t m_on_complete;
};

}
