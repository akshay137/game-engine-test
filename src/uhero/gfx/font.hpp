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
		codepoint code = 0;
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
			border_size = 0.05;
			line_spacing = 1;
			text_color = Color32(255);
			border_color = Color32(0, 0, 0, 255);
		}

		void set_size(i32 pt)
		{
			size = pt * PT_TO_PIXEL;
			line_spacing = 1;
		}
	};

	struct Font
	{
		constexpr static int MAX_GLYPHS = 128;
		Texture atlas;
		i32 line_height;
		i32 space;
		i32 tab_spaces;
		Glyph glyphs[MAX_GLYPHS]; // only ASCII support

		Result create(Texture& atlas, Glyph* glyphs, i32 lh, i32 sp);
		void clear();

		Glyph find_glyph(codepoint code) const;

		float size_scale(u32 size) const { return size / (float)line_height; }

		// text fitting functions
		float fit_size(usize count, const char* buffer, float box_width) const;

		void get_box_size(usize count, const char* buffer, const FontStyle& style,
			float& width, float& height
		) const;
	};
}

#endif