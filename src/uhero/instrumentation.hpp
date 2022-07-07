#ifndef UHERO_INSTRUMENTATION_H__
#define UHERO_INSTRUMENTATION_H__ 1

#include "common.hpp"
#include "time.hpp"
#include "logger.hpp"
#include <string_view>

#ifdef DEBUG
#define UH_PROFILE() auto LINE_VAR(inst) = uhero::Instrument(__PRETTY_FUNCTION__)
#else
#define UH_PROFILE()
#endif

namespace uhero
{
	// measures time between its instance created and destroyed
	// uses RAII
	struct Record
	{
		std::string_view name;
		time_point start;
		time_point end;

		Record(const char* name, time_point tp): name{name}, start{tp}, end{tp} {}

		float get_duration() const
		{
			duration_seconds dur = end - start;
			return dur.count();
		}
	};

	struct Instrument
	{
		Record record;

		Instrument(const char* name): record{name, lib_clock::now()} {}
		~Instrument()
		{
			record.end = lib_clock::now();
			// TODO: store record someplace safe!
			UH_INFO("%.*s: %f\r",
				(int)record.name.size(), record.name.data(),
				record.get_duration()
			);
		}
	};
}

#endif