#include "printer.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>

namespace geiger
{

namespace printer
{

struct console : public printer_base
{
    void on_start(const suite_base& s) override
    {
        std::vector<std::reference_wrapper<const std::string>> names = s.test_names();

        auto it = std::max_element(names.begin(), names.end(), [](const std::string& s1, const std::string& s2)
                                {
                                    return s1.size() < s2.size();
                                });

        m_first_col_width = it->get().size();

        int width = std::fprintf(stdout, "%-*s %12s", m_first_col_width, "Test", "Time (ns)");

        std::vector<int> papi_events = s.papi_events();
        for (int event : papi_events)
        {
            std::string event_name = get_papi_event_name(event);
            width += std::fprintf(stdout, " %12s", event_name.c_str());
        }

        std::cout << "\n" << std::string(width, '-') << std::endl;
    }

    void on_test_complete(const std::string& name, const test_report& r) override
    {
        std::fprintf(stdout, "%-*s %12ld", m_first_col_width, name.c_str(), r.time_per_task().count());

        for (long long counter : r.papi_counters())
            std::fprintf(stdout, " %12lld", counter);

        std::cout << std::endl;
    }

   private:
    int m_first_col_width;
};

}

}
