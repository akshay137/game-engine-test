#ifndef UHERO_GFX_COLOR_H__
#define UHERO_GFX_COLOR_H__ 1

#include "../core/core.h"

UH_TDEF_UNION(color32);

union color32_u
{
	uint32_t color;
	struct { uint8_t a, b, g, r; }; // little endian
};

API_INLINE color32_t color32(float r, float g, float b, float a)
{
	color32_t color = {
		.a = a * 255,
		.b = b * 255,
		.g = g * 255,
		.r = r * 255
	};
	return color;
}

API_INLINE color32_t color32FromCode(uint32_t code)
{
	color32_t color = {
		.color = code
	};
	return color;
}

API_INLINE color32_t color32Swap(color32_t color)
{
	color32_t swapped = {
		.a = color.r,
		.b = color.g,
		.g = color.b,
		.r = color.a
	};
	return swapped;
}

#endif