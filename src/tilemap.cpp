#include "tilemap.hpp"
#include "uhero/file.hpp"
#include "uhero/logger.hpp"
#include "uhero/memory/memory.hpp"

namespace game
{
	uhero::Result TileMap::load_from_file(const char* filename)
	{
		UH_FRAME_STACK_GROUP();
		uhero::File file;
		auto res = file.open(filename,
			uhero::FileMode::FRead | uhero::FileMode::FBinary
		);

		auto id = file.read_leu32();
		UH_VERB("loading tilemap[%u]: %s\n", id, filename);
		
		this->width = file.read_le32();
		this->height = file.read_le32();
		int length = file.read_le32();
		this->tile_count = length;

		UH_VERB("[%dx%d]{%d}\n", this->width, this->height, length);

		this->tiles = UH_ALLOCATE_TYPE(Tile, length);
		for (auto i = 0; i < length; i++)
		{
			Tile tile {};
			tile.id = file.read_le32();
			tile.x = file.read_le32();
			tile.y = file.read_le32();
			// UH_VERB("tile: %d, [%dx%d]\n", tile.id, tile.x, tile.y);
			this->tiles[i] = tile;
		}

		this->tileset.tile_size = file.read_le32();
		this->tileset.columns = file.read_le32();
		length = file.read_le32();
		this->tileset.tile_count = length;
		UH_VERB("tileset: {%d|%d}\n",
			this->tileset.tile_size,
			this->tileset.columns
		);
		this->tileset.tile_flags = UH_ALLOCATE_TYPE(uint32_t, length);
		for (auto i = 0; i < length; i++)
		{
			auto flags = file.read_leu32();
			this->tileset.tile_flags[i] = flags;
		}

		// auto offset = length * sizeof(uint32_t);
		// file.seek(offset, uhero::SeekPosition::Current);

		int width = file.read_le32();
		int height = file.read_le32();
		UH_VERB("atlas: [%dx%d]\n", width, height);

		auto img_size = width * height * 4;
		uint8_t* pixels = UH_FRAME_STACK_ALLOCATE_TYPE(uint8_t, img_size);
		file.read(pixels, img_size);

		this->tileset.atlas.create(width, height,
			uhero::gfx::PixelFormat::RGBA8, pixels
		);

		file.close();
		return uhero::Result::Success;
	}

	void TileMap::clear()
	{
		UH_FREE(tiles);
		UH_FREE(tileset.tile_flags);
		tileset.atlas.clear();
	}
}