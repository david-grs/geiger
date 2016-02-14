#include "printers.h"
#include "benchmark.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>

namespace benchmark
{

namespace printers
{

void console::on_start(const suite_base& s)
{
	std::vector<std::reference_wrapper<const std::string>> names = s.test_names();

	auto it = std::max_element(names.begin(),
							   names.end(),
							   [](const std::string& s1, const std::string& s2) { return s1.size() < s2.size(); });

	m_first_col_width = it->get().size();

	int width = fprintf(stdout, "%-*s %10s %10s", m_first_col_width, "Test", "Time (ns)", "Iterations\n");
	std::cout << std::string(width - 1, '-') << std::endl;
}

void console::on_test_complete(const std::string& name,
							   const test_report& r)
{
	fprintf(stdout, "%-*s %10ld %10ld\n", m_first_col_width, name.c_str(), r.time_per_task().count(), r.iteration_count());
}

}

}
