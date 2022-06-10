#ifndef UHERO_GFX_FONT_H__
#define UHERO_GFX_FONT_H__ 1

#include "../core/core.h"
#include "../math/types.h"
#include "texture.h"
#include "color.h"

#define MAX_GLYPHS_PER_FONT 128

struct glyph_s
{
	uint32_t codepoint;
	ivec2_t position; // position in atlas
	ivec2_t size; // size of bitmap
	ivec2_t bearing; // position relative to base line
	uint32_t advance; // amount to advance for next character
};

// glyph_0 will always be `undefined` glyph
struct font_s
{
	texture_t bitmap;
	uint32_t glyph_count;
	glyph_t glyphs[MAX_GLYPHS_PER_FONT];
	int32_t line_height;
	int32_t space;
};

struct fontstyle_s
{
	int32_t font_size;
	float outline_size;
	color32_t text_color;
	color32_t outline_color;
};

struct vertexglyph_s
{
	vec2_t pos;
	vec2_t uv;
	color32_t text_color;
	color32_t outline_color;
	float width;
	float edge;
};
UH_TDEF_STRUCT(vertexglyph);

API void font_delete(font_t* fnt);

// returns glyph for codepoint if it exist
// otherwise returns glyph for `undefined`
API glyph_t font_findGlyph(font_t* fnt, uint32_t codepoint);

#endif