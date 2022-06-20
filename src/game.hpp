#ifndef GAME_H__
#define GAME_H__ 1

#include "uhero/application.hpp"
#include "uhero/gfx/renderer.hpp"
#include "uhero/gfx/font.hpp"

#include <string_view>
#include <glm/vec2.hpp>

namespace game
{
	struct Game : public uhero::IApplication
	{
		uhero::Context& ctx;
		uhero::gfx::Renderer uber;
		uhero::gfx::Font font;
		uhero::gfx::FontStyle style;

		uhero::gfx::Texture spritesheet;

		Game(uhero::Context& ctx)
			: ctx{ctx}
		{}

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

		glm::vec2 screen_to_world(glm::vec2 pos, glm::vec2 screen);

		glm::vec4 get_spritesheet_source(int x, int y, int size)
		{
			glm::vec4 src(x * size, y * size, size, size);
			return src;
		}

		// for video

		// displays image card in screen space coordinates
		// returns size of the card
		glm::vec2 show_image_card(glm::vec2 pos, float size,
			const uhero::gfx::Texture& texture,
			std::string_view title, bool y_flipped=false
		);

		void show_debug_info();
	};
}

#endif