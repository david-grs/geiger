#pragma once

#include "papi.h"
#include "chrono.h"
#include "printers.h"

#include <boost/optional.hpp>

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <tuple>
#include <chrono>

namespace disco
{

struct test_report
{
    test_report(long iterations, int64_t cycles, std::vector<long long>&& papi_counters = {})
        : m_iterations(iterations), m_cycles(cycles), m_papi_counters(papi_counters)
    {
    }

    long iteration_count() const
    {
        return m_iterations;
    }
    int64_t total_cycles() const
    {
        return m_cycles;
    }

    double cycles_per_task() const
    {
        return m_cycles / (double)m_iterations;
    }
    std::chrono::nanoseconds time_per_task() const
    {
        return chrono::from_cycles(cycles_per_task());
    }

    const std::vector<long long>& papi_counters() const
    {
        return m_papi_counters;
    }

   private:
    long m_iterations;
    int64_t m_cycles;
    std::vector<long long> m_papi_counters;
};

struct test_base
{
    test_base(const std::string& name) : m_name(name)
    {
    }
    virtual ~test_base()
    {
    }

    virtual test_report run(std::chrono::milliseconds duration) const = 0;
    virtual test_report run(long iterations, boost::optional<std::chrono::nanoseconds> duration = boost::none) const = 0;

    const std::string& name() const
    {
        return m_name;
    }

   private:
    std::string m_name;
};

template <typename _CallableT, typename... _PAPIWrappersT>
struct test : public test_base
{
    test(const std::string& name, _CallableT&& callable = _CallableT()) : test_base(name), m_callable(callable)
    {
    }

    test_report run(std::chrono::milliseconds duration) const override;
    test_report run(long iterations, boost::optional<std::chrono::nanoseconds> duration = boost::none) const override;

   private:
    _CallableT m_callable;
};

struct suite_report
{
    std::vector<std::pair<std::string, test_report>> tests;
};

struct suite_base
{
    virtual ~suite_base()
    {
    }

    virtual suite_base& run(std::chrono::milliseconds duration) = 0;
    virtual suite_base& run(long iterations) =0;

    virtual std::vector<std::reference_wrapper<const std::string>> test_names() const = 0;
    virtual std::vector<int> papi_events() const = 0;
};

template <typename... _PAPIWrappersT>
struct suite : public suite_base
{
    suite& run(std::chrono::milliseconds duration = std::chrono::seconds(1)) override;
    suite& run(long iterations) override;

    std::vector<std::reference_wrapper<const std::string>> test_names() const override;
    std::vector<int> papi_events() const override;

    typedef std::function<void(const std::string&, const test_report&)> test_complete_t;
    typedef std::function<void(const suite_report&)> suite_complete_t;

    template <typename _CallableT>
    suite& add(const std::string& name, _CallableT&& callable)
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

   private:
    template <typename _DurationT>
    suite& run_impl(_DurationT duration);

    std::vector<std::unique_ptr<test_base>> m_tests;
    std::unique_ptr<printer_base> m_printer;

    test_complete_t m_on_test_complete;
    suite_complete_t m_on_complete;
};
}

#include "benchmark.tcc"
