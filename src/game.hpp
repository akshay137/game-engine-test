#ifndef GAME_H__
#define GAME_H__ 1

#include "uhero/application.hpp"
#include "uhero/gfx/renderer.hpp"
#include "uhero/gfx/font.hpp"
#include "uhero/gfx/framebuffer.hpp"
#include "tilemap.hpp"

#include <glm/vec2.hpp>

namespace game
{
	struct Game : public uhero::IApplication
	{
		uhero::Context& ctx;
		uhero::gfx::Renderer uber;
		uhero::gfx::Font font;
		uhero::gfx::FontStyle style;
		uhero::gfx::FrameBuffer game_fbo;

		uhero::gfx::Texture tx_characters;
		uhero::gfx::Texture tx_tiles;

		TileMap map;
		glm::vec4 map_camera;

		bool debug_info_enabled = true;

		// constexpr static glm::vec2 GAME_SIZE = { 18 * 32, 18 * 16 };
		constexpr static glm::vec2 GAME_SIZE = { 480, 480 * (9 / 16.0) };

		Game(uhero::Context& ctx)
			: ctx{ctx}
		{}

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

		void draw_tile_map(const TileMap& tmap, glm::vec4 camera);

		glm::vec2 screen_to_world(glm::vec2 pos, glm::vec2 screen);

		glm::vec4 get_spritesheet_source(int x, int y, int size)
		{
			glm::vec4 src(x * size, y * size, size, size);
			return src;
		}

		// debug
		void show_debug_info();
	};
}

#endif