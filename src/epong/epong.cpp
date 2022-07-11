#include "epong.hpp"
#include "../uhero/math/rectangle.hpp"
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
	
	float ring = 0.005;
	float planet =  0;
	void Game::update(float delta)
	{
		auto& ip = ctx->input;
		if (ip.is_key_released(KeyCode::Escape))
		{
			ctx->request_exit();
		}

		if (ip.is_key_down(KeyCode::NP_PLUS))
			ring += delta * .25;
		if (ip.is_key_down(KeyCode::NP_MINUS))
			ring -= delta * .25;
		if (ip.is_key_released(KeyCode::R))
		{
			planet = 0;
			ring = 0.005;
		}
		if (ip.is_key_released(KeyCode::N))
		{
			ring = 0;
		}

		if (ip.is_key_down(KeyCode::ArrowUp))
			planet += delta;
		if (ip.is_key_down(KeyCode::ArrowDown))
			planet -= delta;
		if (ip.is_key_released(KeyCode::D1))
			planet = glm::radians(45.0f);
		if (ip.is_key_released(KeyCode::D2))
			planet = glm::radians(45.0f * 2);
		if (ip.is_key_released(KeyCode::D3))
			planet = glm::radians(45.0f * 3);
		if (ip.is_key_released(KeyCode::D4))
			planet = glm::radians(45.0f * 4);
		if (ip.is_key_released(KeyCode::D5))
			planet = glm::radians(45.0f * 5);
		if (ip.is_key_released(KeyCode::D6))
			planet = glm::radians(45.0f * 6);
		if (ip.is_key_released(KeyCode::D7))
			planet = glm::radians(45.0f * 7);
		if (ip.is_key_released(KeyCode::D8))
			planet = glm::radians(45.0f * 8);
	}
	
	void Game::render()
	{
		glm::vec2 screen_size = { ctx->main_window.width, ctx->main_window.height };
		uber.set_clip_size(screen_size.x, screen_size.y);
		ctx->gfx.clear_buffer(gfx::BLACK);

		float delta = ctx->main_clock.delta();
		float time = ctx->main_clock.seconds();

		constexpr float DIAMETER = 512;
		constexpr float RADIUS = DIAMETER * .5;
		glm::vec2 planet_pos = glm::normalize(glm::vec2(1)) * RADIUS;
		float s = glm::sin(planet);
		float c = glm::cos(planet);
		auto ppos = planet_pos;
		ppos.x = planet_pos.x * c - planet_pos.y * s;
		ppos.y = planet_pos.x * s + planet_pos.y * c;
		auto center = screen_size * .5f;
		uber.draw_color(center, glm::vec2(DIAMETER), gfx::CYAN, 0, ring);
		uber.draw_color(center + planet_pos, glm::vec2(DIAMETER * .1f),
			gfx::WHITE.with_alpha(.7), 0, 1
		);
		uber.draw_color(center + ppos, glm::vec2(DIAMETER * .1f), gfx::RED, 0, 1);

		// axis
		// original
		auto color = gfx::GREEN;
		float thickness = 3;
		uber.draw_color(center, { 1000, thickness }, color);
		uber.draw_color(center, { thickness, 1000 }, color);

		// from planet
		color = gfx::YELLOW;
		auto rect = math::Rectangle::from_coords(center + ppos, { thickness, -ppos.y });
		uber.draw_color(rect.position, rect.size, color);
		rect = math::Rectangle::from_coords(center + ppos, { -ppos.x, thickness });
		uber.draw_color(rect.position, rect.size, color);

		// debug info
		auto style = gfx::FontStyle(16);
		style.text_color = gfx::WHITE;
		style.border_size = 0.05;
		style.border_color = gfx::BLACK;
		uber.write_format(center + ppos, font, style,
			"-planet: (%f)\n [%.2f, %.2f]\n", planet, ppos.x, ppos.y
		);
		glm::vec2 pen = glm::vec2(0.0f, screen_size.y - 0);
		pen = uber.write_format(pen, font, style,
			"delta: %.4f | time: %f\n",
			delta, time
		);
		pen = uber.write_format(pen, font, style, "ring: %f\n", ring);
		// pen = uber.write_format(pen, font, style,
		// 	"planet: (%f)[%.2f, %.2f]\n", planet, ppos.x, ppos.y
		// );
		pen = uber.write_format(pen, font, style,
			"(x, y)[%.2f, %.2f]\n"
			"sin(planet) = %f\n"
			"cos(planet) = %f\n"
			"x = (x * cos)[%f] - (y * sin)[%f]\n"
			"y = (x * sin)[%f] + (y * cos)[%f]\n"
			"\n",
			planet_pos.x, planet_pos.y,
			s, c,
			planet_pos.x * c, planet_pos.y * s,
			planet_pos.x * s, planet_pos.y * c
		);
		// style.set_size(128);
		// pen = uber.write_format(pen, font, style, "Hello World");
		uber.flush();
	}
}