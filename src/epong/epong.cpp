#include "epong.hpp"
#include "../uhero/uhero.hpp"
#include "../uhero/res/font_atlas.hpp"
#include "../uhero/res/texture.hpp"

#include <glm/glm.hpp>

using namespace uhero;

namespace epong
{
	gfx::Texture map;

	uhero::Result Game::load(Context& ctx)
	{
		auto res = uber.create(2048);
		font = res::load_font("assets/firacode.atlas");
		map = res::load_texture("map.png");

		this->ctx = &ctx;
		return Result::Success;
	}

	void Game::clear()
	{
		map.clear();
		font.clear();
		uber.clear();
	}
	
	float ring = 0;
	void Game::update(float delta)
	{
		auto& ip = ctx->input;
		if (ip.is_key_released(KeyCode::Escape))
		{
			ctx->request_exit();
		}

		if (ip.is_key_down(KeyCode::NP_PLUS))
			ring += delta;
		if (ip.is_key_down(KeyCode::NP_MINUS))
			ring -= delta;
		if (ip.is_key_released(KeyCode::R))
			ring = 0;
	}
	
	void Game::render()
	{
		glm::vec2 screen_size = { ctx->main_window.width, ctx->main_window.height };
		uber.set_clip_size(screen_size.x, screen_size.y);
		ctx->gfx.clear_buffer(gfx::BLACK);

		float delta = ctx->main_clock.delta();
		float time = ctx->main_clock.seconds();
		// ring = glm::sin(time);
		uber.draw_color(screen_size * .5f, glm::vec2(512), gfx::CYAN, 0, ring);

		// debug info
		auto style = gfx::FontStyle(16);
		style.text_color = gfx::WHITE;
		style.border_size = 0.05;
		style.border_color = gfx::BLACK;
		glm::vec2 pen = glm::vec2(0.0f, screen_size.y - 0);
		pen = uber.write_format(pen, font, style,
			"delta: %.4f | time: %f\n",
			delta, time
		);
		pen = uber.write_format(pen, font, style, "ring: %f\n", ring);
		style.set_size(128);
		pen = uber.write_format(pen, font, style, "Hello World");
		uber.flush();
	}
}