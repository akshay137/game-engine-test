#include "font_atlas.hpp"
#include "../file.hpp"
#include "../memory/memory.hpp"
#include "../logger.hpp"

namespace uhero::res
{
	gfx::Font load_font(const char* filename)
	{
		assert(filename);

		File file;
		auto res = file.open(filename, FileMode::FRead | FileMode::FBinary);
		if (Result::Success != res)
		{
			UH_ERROR("Failed to load font: %s\n", filename);
			assert(false);
			return {};
		}

		gfx::Font font {};
		auto id = file.read_leu32();
		if (0x00aabb00 != id)
		{
			UH_ERROR("%s is not a font atlas file\n", filename);
			file.close();
			return {};
		}

		auto glyph_count = file.read_leu32();
		font.line_height = file.read_le32();
		i32 w = file.read_le32(); // width
		i32 h = file.read_le32(); // height

		// font.glyphs = UH_ALLOCATE_TYPE(gfx::Glyph, font.glyph_count);
		for (u32 i = 0; i < glyph_count; i++)
		{
			gfx::Glyph g{};

			g.code = file.read_leu32();
			g.pos_x = file.read_le32();
			g.pos_y = file.read_le32();
			g.size_x = file.read_le32();
			g.size_y = file.read_le32();
			g.bearing_x = file.read_le32();
			g.bearing_y = file.read_le32();
			g.advance_x = file.read_leu32();

			font.glyphs[g.code] = g;
		}

		// read image
		usize image_size = w * h;
		UH_FRAME_STACK_GROUP();
		void* image = UH_FRAME_STACK_ALLOCATE(image_size);

		file.read(image, image_size);

		font.atlas.create(w, h, gfx::PixelFormat::GREYSCALE, image, 3);
		font.atlas.set_filter(gfx::TextureFilter::BiLinear);
		// font.line_height = font.glyphs[0].size_y;
		font.space = font.glyphs[0].advance_x;

		UH_VERB("FONT_ATLAS: %s: [GC: %u | <%dx%d> | HxW: <%dx%d>]\n",
			filename,
			glyph_count, w, h,
			font.line_height, font.space
		);
		file.close();

		return font;
	}
}