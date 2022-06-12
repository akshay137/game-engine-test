#include "font.h"

void font_delete(font_t* fnt)
{
	texture_delete(&(fnt->bitmap));
}

glyph_t font_findGlyph(font_t* fnt, uint32_t codepoint)
{
	glyph_t res = fnt->glyphs[0];

	uint32_t start = 0;
	uint32_t end = fnt->glyph_count;
	const glyph_t* list = fnt->glyphs;

	while (start < end)
	{
		uint32_t mid = start + (end - start) / 2;
		if (list[mid].codepoint == codepoint)
			return list[mid];
		
		if (list[mid].codepoint > codepoint)
			end = mid;
		else
			start = mid;
	}

	return res;

	// // TODO: change search method
	// for (uint32_t i = 0; i < fnt->glyph_count; i++)
	// {
	// 	const glyph_t* glyph = &(fnt->glyphs[i]);
	// 	if (codepoint == glyph->codepoint)
	// 		return *glyph;
	// }

	return res;
}
