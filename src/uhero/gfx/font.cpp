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
	}

	void Font::clear(bool free_glyphs)
	{
		if (free_glyphs)
			UH_FREE(glyphs);
		atlas.clear();
	}

	Glyph Font::find_glyph(codepoint code) const
	{}
}