#ifndef GAME_H__
#define GAME_H__ 1

#include "uhero/application.hpp"
#include "uhero/gfx/renderer.hpp"
#include "uhero/gfx/font.hpp"
#include "uhero/gfx/framebuffer.hpp"
#include "tilemap.hpp"

#include <Box2D/Box2D.h>
#include <glm/vec2.hpp>

namespace game
{
	struct Game : public uhero::IApplication
	{
		uhero::Context& ctx;
		uhero::gfx::Renderer uber;
		uhero::gfx::Font font;
		uhero::gfx::FontStyle style;

		float s2v_ratio = 50;
		b2World* world;
		b2Body* ground;
		b2Body* ball;

		bool debug_info_enabled = true;

		Game(uhero::Context& ctx)
			: ctx{ctx}
		{}

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

		glm::vec2 screen_to_world(glm::vec2 pos, glm::vec2 screen)
		{
			return glm::vec2(pos.x, screen.y - pos.y);
		}

		// debug
		void show_debug_info();
	};
}

#endif