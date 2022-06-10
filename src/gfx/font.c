#include "font.h"

void font_delete(font_t* fnt)
{
	texture_delete(&(fnt->bitmap));
}

glyph_t font_findGlyph(font_t* fnt, uint32_t codepoint)
{
	glyph_t res = fnt->glyphs[0];

	// TODO: change search method
	for (uint32_t i = 0; i < fnt->glyph_count; i++)
	{
		const glyph_t* glyph = &(fnt->glyphs[i]);
		if (codepoint == glyph->codepoint)
			return *glyph;
	}

	return res;
}
