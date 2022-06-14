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
		u32 end = glyph_count;

		while (start <= end)
		{
			u32 mid = start + (end - start) / 2;

			if (glyphs[mid].code == code)
				return glyphs[mid];
			
			if (glyphs[mid].code > code)
				end = mid;
			else
				start = mid;
		}

		return g;
	}
}