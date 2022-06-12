#include "fontatlas.h"
#include "file.h"
#include "../core/logger.h"

#include <malloc.h>

result_t font_loadAtlas(font_t* out_fnt, const char* filename)
{
	font_t fnt = {};
	file_t file = uh_openFile(filename, UHFILE_READ | UHFILE_BINARY);

	uint32_t id = uh_readFileU32(&file);
	if (UH_FONTATLAS_ID != id)
	{
		uh_closeFile(&file);
		return UH_FAILURE;
	}

	fnt.glyph_count = uh_readFileU32(&file);
	ivec2_t bitmap_size = {};
	bitmap_size.x = uh_readFileI32(&file);
	bitmap_size.y = uh_readFileI32(&file);

	// read glyph data
	for (uint32_t i = 0; i < fnt.glyph_count; i++)
	{
		glyph_t g = {};

		g.codepoint = uh_readFileU32(&file);
		g.position.x = uh_readFileI32(&file);
		g.position.y = uh_readFileI32(&file);
		g.size.x = uh_readFileI32(&file);
		g.size.y = uh_readFileI32(&file);
		g.bearing.x = uh_readFileI32(&file);
		g.bearing.y = uh_readFileI32(&file);
		g.advance = uh_readFileU32(&file);

		fnt.glyphs[i] = g;
	}

	fnt.space = fnt.glyphs[0].advance * 0.7;
	fnt.line_height = fnt.glyphs[0].size.y;

	INFO("FontAtlas: %s\nGlyphCount: %d | BitmapSize: %d\n"
		"LineHeight: %d | Space: %d\n",
		filename, fnt.glyph_count, bitmap_size.x,
		fnt.line_height, fnt.space
	);

	// read bitmap
	size_t buffer_size = bitmap_size.x * bitmap_size.y;
	void* buffer = malloc(buffer_size);
	size_t read = uh_readFile(&file, buffer, buffer_size);
	result_t res = texture_new(&(fnt.bitmap), UH_GREYSCALE8,
		bitmap_size.x, bitmap_size.y, buffer
	);
	free(buffer);
	uh_closeFile(&file);

	if (UH_SUCCESS != res)
	{
		ERROR("Failed to create texture for font: %d\n", res);
		return res;
	}
	texture_setFiltering(&(fnt.bitmap), UH_FILTER_LINEAR);

	*out_fnt = fnt;

	return UH_SUCCESS;
}