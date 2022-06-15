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

	constexpr f32 PT_TO_PIXEL = 1.328147f;
	struct FontStyle
	{
		f32 size;
		f32 border_size;
		f32 line_spacing;
		Color32 text_color;
		Color32 border_color;
		i32 wrap_width;

		FontStyle(f32 size=16)
			: size{size * PT_TO_PIXEL}
		{
			wrap_width = 0;
			border_size = 0.01f;
			line_spacing = 0;
			text_color = Color32(255);
			border_color = Color32(0, 0, 0, 255);
		}
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

		float size_scale(u32 size) const { return size / (float)line_height; }
	};
}

#endif