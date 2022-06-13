#ifndef UHERO_GFX_FONT_H__
#define UHERO_GFX_FONT_H__ 1

#include "../common.hpp"
#include "texture.hpp"
#include "color.hpp"

namespace uhero::gfx
{
	using codepoint = u32;

	struct Glyph
	{
		codepoint code;
		i32 pos_x;
		i32 pos_y;
		i32 size_x;
		i32 size_y;
		i32 bearing_x;
		i32 bearing_y;
		u32 advance_x;
	};

	struct FontStyle
	{
		i32 size;
		f32 outline_size;
		Color32 text_color;
		Color32 border_color;
	};

	struct Font
	{
		Texture atlas;
		i32 line_height;
		i32 space;
		u32 glyph_count;
		Glyph* glyphs;

		Result create(Texture& atlas, u32 count, Glyph* glyphs, i32 lh, i32 sp);
		void clear(bool free_glyphs = true);

		Glyph find_glyph(codepoint code) const;
	};
}

#endif