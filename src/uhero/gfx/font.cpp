#include "font.hpp"
#include "../logger.hpp"
#include "../memory/memory.hpp"

namespace uhero::gfx
{
	Result Font::create(Texture& atlas, u32 count, Glyph* glyphs, i32 lh, i32 sp)
	{
		this->atlas = atlas;
		glyph_count = count;
		this->glyphs = glyphs;
		line_height = lh;
		space = sp;
		tab_spaces = 4;

		return Result::Success;
	}

	void Font::clear(bool free_glyphs)
	{
		if (free_glyphs)
			UH_FREE(glyphs);
		atlas.clear();
	}

	Glyph Font::find_glyph(codepoint code) const
	{
		assert(glyphs);

		Glyph g = glyphs[0];

		u32 start = 0;
		u32 end = glyph_count - 1;

		while (start <= end)
		{
			u32 mid = (start + end) / 2;

			if (glyphs[mid].code == code)
				return glyphs[mid];
			
			if (glyphs[mid].code > code)
				end = mid - 1;
			else
				start = mid + 1;
		}

		return g;
	}

	float Font::fit_size(usize count, const char* buffer, float size) const
	{
		FontStyle tstyle(16);
		float width, height;
		get_box_size(count, buffer, tstyle, width, height);
		return (tstyle.size / PT_TO_PIXEL) * (size / width);
	}

	void Font::get_box_size(usize count, const char* buffer, const FontStyle& style,
		float& width, float& height
	) const
	{
		width = 0;
		height = 0;

		float line_width = 0;
		float scale = size_scale(style.size);
		float final_line_height = line_height + style.line_spacing * scale;
		i32 line = 1;

		for (usize i = 0; i < count; i++)
		{
			char c = buffer[i];

			if ('\n' == c) // new line
			{
				if (width < line_width) width = line_width;
				line_width = 0;
				line++;
				continue;
			}
			if (' ' == c)
			{
				line_width += space * scale;
			}
			if ('\t' == c)
			{
				line_width += tab_spaces * scale;
			}

			const Glyph& g = find_glyph(c);
			line_width += g.advance_x * scale;
		}

		if (width < line_width) width = line_width;
		height = line * final_line_height;
	}
}