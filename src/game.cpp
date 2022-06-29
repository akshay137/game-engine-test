#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/font_atlas.hpp"

#include <cmath>

namespace game
{
	using namespace uhero;

	glm::vec2 snap_to_pixel(glm::vec2 pos)
	{
		auto res = glm::vec2(round(pos.x), round(pos.y));
		return res;
	}

	bool Game::set_minigame(MiniGame* mgame)
	{
		if (current_game)
			current_game->cleanup();
		
		current_game = mgame;
		return current_game->setup(*this, ctx.main_window.width, ctx.main_window.height);
	}

	glm::vec2 Game::get_window_size() const
	{
		return glm::vec2(ctx.main_window.width, ctx.main_window.height);
	}

	uhero::Result Game::load(uhero::Context&)
	{
		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(16);
		style.border_size = 0.1;
		style.border_color = gfx::Color32::from_rgba(1, 0, 0);

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		if (current_game) current_game->cleanup();
		uber.clear();
		font.clear();
	}

	void Game::update(float delta)
	{
		auto& ip = ctx.input;
		auto& audio = ctx.audio;

		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();
		if (ip.is_key_released(KeyCode::Tilde))
			debug_info_enabled = !debug_info_enabled;
		
		if (current_game) current_game->update(*this, delta);
	}

	void Game::render()
	{
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);

		if (current_game) current_game->draw(*this);

		uber.flush();

		// game debug info
		if (debug_info_enabled)
			show_debug_info();
		
		auto pen = screen_to_world(glm::vec2(0), screen);
		uber.flush();
	}

	void Game::show_debug_info()
	{
		auto stats = gfx::Context::gpu_stats;

		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto time = ctx.main_clock.nanoseconds();
		i32 ms = time / 1000000;
		i32 seconds = ms / 1000;
		i32 minutes = seconds / 60;
		i32 hours = minutes / 60;

		minutes %= 60;
		seconds %= 60;
		ms %= 1000;

		auto pen = glm::vec2(8, 64 + 32 + 16);
		auto _style = style;
		_style.size = 15.0 * gfx::PT_TO_PIXEL;
		_style.border_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_size = 0.1;
		uber.draw_color(glm::vec2(104, 60), glm::vec2(200, 108), gfx::Color32::from_rgba(0, 1, 0));
		pen = uber.write_format(pen, font, _style, "Time: %d:%d:%d:%d\n",
			hours, minutes, seconds, ms / 100
		);
		pen = uber.write_format(pen, font, _style, "Delta: %f\n", ctx.main_clock.delta());
		pen = uber.write_format(pen, font, _style,
			"Draw calls: %u\nTriangles: %u\nTextureSwitch: %u",
			drawcalls, triangles, switches
		);

		uber.flush();
	}
}