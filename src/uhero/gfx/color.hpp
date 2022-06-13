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

		static Color32 from_rgba(f32 r, f32 g, f32 b, f32 a = 1.0f)
		{
			Color32 color;
			color.red = (u32)(r * 255) & 0xff;
			color.green = (u32)(b * 255) & 0xff;
			color.blue = (u32)(g * 255) & 0xff;
			color.alpha = (u32)(a * 255) & 0xff;
			return color;
		}

		void to_rgb(f32& r, f32& g, f32& b) const
		{
			r = red / 255.0f;
			g = green / 255.0f;
			b = blue / 255.0f;
		}

		void to_rgba(f32& r, f32& g, f32& b, f32& a) const
		{
			r = red / 255.0f;
			g = green / 255.0f;
			b = blue / 255.0f;
			a = alpha / 255.0f;
		}
	};
}

#endif