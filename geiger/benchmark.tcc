#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <boost/fusion/include/accumulate.hpp>

#include <iterator>

namespace geiger
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
suite<_PAPIWrappersT...>& suite<_PAPIWrappersT...>::run(std::chrono::milliseconds duration)
{
    return run_impl(duration);
}

template <typename... _PAPIWrappersT>
suite<_PAPIWrappersT...>& suite<_PAPIWrappersT...>::run(long iterations)
{
    return run_impl(iterations);
}

template <typename... _PAPIWrappersT>
template <typename _DurationT>
suite<_PAPIWrappersT...>& suite<_PAPIWrappersT...>::run_impl(_DurationT duration)
{
    suite_report r;

    if (m_printer)
        m_printer->on_start(*this);

    for (const auto& p : m_tests)
    {
        test_report test_report = p->run(duration);
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
test_report test<_CallableT, _PAPIWrappersT...>::run(std::chrono::milliseconds duration) const
{
    test_report r = run(1);

    assert((r.iteration_count() == 1 && sizeof...(_PAPIWrappersT) == 0)
           || r.iteration_count() == sizeof...(_PAPIWrappersT));

    auto time_elapsed = r.time_per_task();
    if (time_elapsed > duration)
        return r;

    long iterations = duration / time_elapsed;
    return run(iterations, std::chrono::nanoseconds(duration));
}

template <typename _CallableT, typename... _PAPIWrappersT>
test_report test<_CallableT, _PAPIWrappersT...>::run(long iterations,
                                                     boost::optional<std::chrono::nanoseconds> duration) const
{
    long batches;

    if (!duration)
    {
        batches = 1;
    }
    else
    {
        // TODO
        if (iterations > 1e6)
            batches = 1e3;
        else if (iterations > 1e4)
            batches = 100;
        else if (iterations > 1e3)
            batches = 10;
        else
            batches = 1;
    }

    iterations /= batches;
    chrono c;

    auto run_benchmark = [&]() -> auto
    {
        c.restart();

        for (long i = 0; i < iterations; ++i)
            m_callable();

        return c.elapsed();
    };

    auto next_iterations_count = [&](int64_t cycles_last_batch)
    {
        if (duration)
        {
            int64_t expected_cycles = chrono::to_cycles(duration.get() / batches);
            double calibration = cycles_last_batch / (double)expected_cycles;

            iterations = std::lround(iterations / calibration);
        }
    };

    std::tuple<_PAPIWrappersT...> papi_wrappers;
    constexpr long papi_wrapppers_count = sizeof...(_PAPIWrappersT);

    int64_t total_cycles = 0;
    long total_iterations = 0;

    if (papi_wrapppers_count == 0)
    {
        for (int i = 0; i < batches; ++i)
        {
            int64_t cycles = run_benchmark();

            total_iterations += iterations;
            total_cycles += cycles;
            next_iterations_count(cycles);
        }

        return {total_iterations, total_cycles};
    }

    int hwd_counters = boost::fusion::accumulate(papi_wrappers, 0, [&](int count, auto& papi) { return count + papi.events_count; });

    std::vector<long long> counters;
    counters.reserve(hwd_counters);

    boost::fusion::for_each(papi_wrappers, [&](auto& papi)
                            {
                                std::vector<long long> curr_counters(papi.events_count);

                                for (int i = 0; i < batches; ++i)
                                {
                                    papi.start();
                                    int64_t cycles = run_benchmark();
                                    papi.stop();

                                    total_iterations += iterations;
                                    total_cycles += cycles;
                                    next_iterations_count(cycles);

                                    const auto& batch_counters = papi.get_counters();

                                    for (int j = 0; j < (int)curr_counters.size(); ++j)
                                        curr_counters[j] += batch_counters[j];
                                }

                                std::copy(curr_counters.begin(),
                                          curr_counters.end(),
                                          std::back_inserter(counters));
                            });

    return {total_iterations, total_cycles, std::move(counters)};
}

}
