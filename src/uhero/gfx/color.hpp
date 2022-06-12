#ifndef UHERO_GFX_COLOR_H__
#define UHERO_GFX_COLOR_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	// RGBA color value (32 bit)
	struct Color32
	{
		u8 red;
		u8 green;
		u8 blue;
		u8 alpha;

		Color32() = default;

		Color32(u8 v)
			: red{v}, green{v}, blue{v}, alpha{v}
		{}

		Color32(u8 r, u8 g, u8 b)
			: red{r}, green{g}, blue{b}, alpha{1}
		{}

		Color32(u8 r, u8 g, u8 b, u8 a)
			: red{r}, green{g}, blue{b}, alpha{a}
		{}

		void to_float(float& r, float& g, float& b, float& a) const
		{
			r = red / 255.0f;
			g = green / 255.0f;
			b = blue / 255.0f;
			a = alpha / 255.0f;
		}
	};
}

#endif