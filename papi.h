#pragma once

#include <papi.h>

#include <cstddef>
#include <array>
#include <string>

namespace benchmark
{

static std::string get_papi_event_name(int event_code)
{
	char event_name[PAPI_MAX_STR_LEN];
	PAPI_event_code_to_name(event_code, event_name);

	return event_name;
}

struct papi_wrapper_base
{
	virtual void start() =0;
	virtual void stop() =0;
};

template <int... _EventsT>
struct papi_wrapper : public papi_wrapper_base
{
	static constexpr int events_count = sizeof...(_EventsT);
	typedef std::array<long long, events_count> counters_type;

	static const std::array<int, events_count>& get_event_types() { return s_events; }

	void start() override
	{
		int ret;
		if ((ret = ::PAPI_start_counters(const_cast<int*>(s_events.data()), events_count)) != PAPI_OK)
			throw std::runtime_error(PAPI_strerror(ret));
	}

	void stop() override
	{
		int ret;

		if ((ret = PAPI_stop_counters(&m_counters[0], events_count)) != PAPI_OK)
			throw std::runtime_error(PAPI_strerror(ret));
	}

	const counters_type& get_counters() const { return m_counters; }

	template <int _EventIndexT>
	long long get_counter() const { return m_counters[_EventIndexT]; }

	template <int _EventIndexT>
	static constexpr int get_event_type() { return s_events[_EventIndexT]; }

	template <int _EventIndexT>
	static const std::string& get_event_name() { return s_event_names[_EventIndexT]; }

private:
	static constexpr std::array<int, events_count> s_events = {{_EventsT...}};
	static const std::array<std::string, events_count> s_event_names;

	counters_type m_counters;
};

template <int... _EventsT> constexpr std::array<int, papi_wrapper<_EventsT...>::events_count> papi_wrapper<_EventsT...>::s_events;

template <std::size_t _SizeT>
static auto get_papi_event_names(const std::array<int, _SizeT>& events)
{
	std::array<std::string, _SizeT> ret;
	for (std::size_t i = 0; i < ret.size(); ++i)
		ret[i] = get_papi_event_name(events[i]);

	return ret;
}

template <int... _EventsT> const std::array<std::string, papi_wrapper<_EventsT...>::events_count> papi_wrapper<_EventsT...>::s_event_names
	= get_papi_event_names(papi_wrapper<_EventsT...>::s_events);

typedef papi_wrapper<PAPI_L1_DCM, PAPI_L2_DCM, PAPI_L3_TCM> cache_profiler;
typedef papi_wrapper<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_BR_MSP> instr_profiler;

}

