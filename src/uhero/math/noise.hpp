#ifndef UHERO_MATH_NOISE_H__
#define UHERO_MATH_NOISE_H__ 1

#include "../common.hpp"

namespace uhero::math
{
	// noise function from GDC talk: https://www.youtube.com/watch?v=LWFzPP8ZbdU
	// video title: Noise-Based RNG
	UHAPI u32 squirrel_noise_impl(u32 v, u32 seed);

	UHINLINE u32 noise(u32 v, u32 seed=0)
	{
		return squirrel_noise_impl(v, seed);
	}

	UHINLINE u64 noise64(u64 v, u64 seed=0)
	{
		constexpr u32 MASK = ~static_cast<u32>(0);
		u32 left = squirrel_noise_impl(v & MASK, seed & MASK);
		u32 right = squirrel_noise_impl(v >> 32, seed >> 32);
		u64 res = ((u64)right << 32) | left;
		return res;
	}

	UHINLINE float noisef(u32 v, u32 seed=0)
	{
		constexpr u32 MAX = ~static_cast<u32>(0);
		auto res = squirrel_noise_impl(v, seed);
		return res / (float)MAX;
	}

	template <typename T>
	T noise_range(u32 v, u32 seed, const T& lower, const T& upper)
	{
		auto tmp = noisef(v, seed);
		T res = lower + (upper - lower) * tmp;
		return res;
	}
}

#endif