#ifndef UHERO_SFX_UNITS_H__
#define UHERO_SFX_UNITS_H__ 1

#include "../common.hpp"
#include <cmath>

namespace uhero::sfx
{
	struct Units
	{
		static i32 khz_to_hz(float khz) { return khz * 1000; }
		static i32 mhz_to_hz(float mhz) { return mhz * 1000 * 1000; }

		static i32 samples_per_milliseconds(i32 frequency, i32 milliseconds)
		{
			f32 seconds = milliseconds / 1000.0f;
			i32 samples = frequency * seconds;
			return samples;
		}

		static float to_decible(float sample)
		{
			float l = log10f(sample);
			float decibles = l * 20;
			return decibles;
		}

		static float decible_to_float(float db)
		{
			float d20 = db / 20.0f;
			float linear = powf(10.0f, d20);
			return linear;
		}
	};
}

#endif