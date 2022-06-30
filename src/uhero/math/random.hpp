#ifndef UHERO_MATH_RANDOM_H__
#define UHERO_MATH_RANDOM_H__ 1

#include "noise.hpp"

namespace uhero::math
{
	// Noise based random number generator
	struct Random
	{
		u32 seed;
		u32 index;

		Random(): seed{0}, index{0} {}
		Random(u32 seed): seed{seed}, index{0} {}

		u32 next()
		{
			return squirrel_noise_impl(index++, seed);
		}

		u32 at(u32 index) const
		{
			return squirrel_noise_impl(index, seed);
		}

		template <typename T>
		T range(const T& lower, const T& upper)
		{
			return noise_range(index++, seed, lower, upper);
		}
	};
}

#endif