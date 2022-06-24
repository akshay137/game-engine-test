#ifndef GAME_TILEMAP_H__
#define GAME_TILEMAP_H__ 1

#include "uhero/common.hpp"
#include "uhero/gfx/texture.hpp"

namespace game
{
	enum TileFlags : uint32_t
	{
		Static = 1 << 0,
		Interactable = 1 << 1,
		Collect = 1 << 2,
	};

	struct Tile
	{
		int id;
		int x, y; // position
	};

	struct TileSet
	{
		int32_t columns;
		int32_t tile_size;
		int32_t tile_count;
		uint32_t* tile_flags;
		uhero::gfx::Texture atlas;

		bool get_texture_offset(int id, int& x, int & y) const
		{
			if (id >= tile_count) return false;
			auto row = id / columns;
			auto column = id % columns;
			x = column * tile_size;
			y = row * tile_size;
			return true;
		}
	};

	struct TileMap
	{
		int32_t width;
		int32_t height;
		TileSet tileset;
		int32_t tile_count;
		Tile* tiles;

		uhero::Result load_from_file(const char* filename);
		void clear();

		uint32_t get_tile_flags(int id) const
		{
			return tileset.tile_flags[id];
		}

		bool tile_has_flag(int id, TileFlags flag) const
		{
			auto flags = get_tile_flags(id);
			return static_cast<uint32_t>(flag) & flags;
		}

		int32_t get_world_width() const
		{
			auto size = width * tileset.tile_size;
			return size;
		}

		int32_t get_world_height() const
		{
			auto size = height * tileset.tile_size;
			return size;
		}
	};
}

#endif