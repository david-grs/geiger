#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

namespace benchmark
{

template <typename... _PAPIWrappersT>
std::vector<std::reference_wrapper<const std::string>> suite<_PAPIWrappersT...>::test_names() const
{
	std::vector<std::reference_wrapper<const std::string>> v;

	for (const auto& test : m_tests)
		v.push_back(test->name());

	return v;
}

template <typename... _PAPIWrappersT>
std::vector<int> suite<_PAPIWrappersT...>::papi_events() const
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

template <typename... _PAPIWrappersT>
suite<_PAPIWrappersT...>& suite<_PAPIWrappersT...>::run()
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

template <typename _CallableT, typename... _PAPIWrappersT>
test_report test<_CallableT, _PAPIWrappersT...>::run() const
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

}

