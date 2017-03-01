#include "printer.h"
#include "benchmark.h"

#include <fstream>
#include <string>

namespace geiger
{

namespace printer
{

struct csv : public printer_base
{
    csv(const std::string& filename, char delimiter = ';')
     : m_filename(filename),
       m_delimiter(delimiter)
    {
    }

    csv(const csv& c) =delete;

    csv(csv&& c)
    : m_filename(std::move(c.m_filename)),
      m_delimiter(c.m_delimiter)
    {
    }

    void on_start(const suite_base& s) override
    {
        // Not the first run(): header has already been written, return
        if (m_ofile.is_open())
            return;

        m_ofile.open(m_filename.c_str());

        if (!m_ofile.is_open())
            throw std::runtime_error("geiger::printer::csv: unable to open file " + m_filename);

        m_ofile << "#Test" << m_delimiter << "Time" << m_delimiter << "Iterations";
        (void)s;
        
#ifdef USE_PAPI

        std::vector<int> papi_events = s.papi_events();
        for (auto it = papi_events.begin(); it != papi_events.end(); ++it)
        {
            m_ofile << m_delimiter << get_papi_event_name(*it);
        }

#endif
    }

    void on_test_complete(const std::string& name, const test_report& r) override
    {
        m_ofile << "\n" << name << m_delimiter << r.time_per_task().count() << m_delimiter << r.iteration_count();

        for (long long counter : r.papi_counters())
        {
            m_ofile << m_delimiter << counter;
        }
    }

private:
    std::ofstream m_ofile;

    std::string m_filename;
    char        m_delimiter;
};

}

}
