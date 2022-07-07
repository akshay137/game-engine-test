#ifndef UHERO_TIME_H__
#define UHERO_TIME_H__ 1

#include "common.hpp"

#include <chrono>
#include <iostream>

namespace uhero
{
	using lib_clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<lib_clock>;
	using duration_seconds = std::chrono::duration<double>;
	using duration_milli = std::chrono::duration<i64, std::milli>;
	using duration_micro = std::chrono::duration<i64, std::micro>;
	using duration_nano = std::chrono::duration<i64, std::nano>;

	struct Clock
	{
		time_point start;
		duration_seconds elapsed_seconds;
		duration_nano elapsed_nanoseconds;
		time_point last_tick;
		duration_seconds tick_duration;

		void reset()
		{
			start = lib_clock::now();
			elapsed_seconds = start - start;
			elapsed_nanoseconds = start - start;
			tick();
		}

		void tick()
		{
			auto temp = lib_clock::now();
			elapsed_nanoseconds = temp - start;
			elapsed_seconds = temp - start;
			tick_duration = temp - last_tick;
			last_tick = temp;
		}

		double seconds() const
		{
			return elapsed_seconds.count();
		}

		i64 milliseconds() const
		{
			return elapsed_nanoseconds.count() / 1000000;
		}

		i64 nanoseconds() const
		{
			return elapsed_nanoseconds.count();
		}

		double delta() const
		{
			return tick_duration.count();
		}

		// frame timestamp, should be used for per frame calculations
		i64 timestamp() const
		{
			return elapsed_nanoseconds.count();
		}

		// timestamp at current moment, more accurate
		i64 timestamp_exact() const
		{
			auto temp = lib_clock::now();
			duration_nano ts = temp - start;
			return ts.count();
		}
	};
}

#endif