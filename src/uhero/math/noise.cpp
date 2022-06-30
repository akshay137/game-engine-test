#include "noise.hpp"

namespace uhero::math
{
	u32 squirrel_noise_impl(u32 v, u32 seed)
	{
		constexpr u32 PRIME_1 = 3039394381u;
		constexpr u32 PRIME_2 = 1759714724u;
		constexpr u32 PRIME_3 = 458671337u;

		u32 noise = v;
		noise *= PRIME_1;
		noise += seed;
		noise ^= (noise >> 8);
		noise += PRIME_2;
		noise ^= (noise << 8);
		noise *= PRIME_3;
		noise ^= (noise >> 8);
		return noise;
	}
}