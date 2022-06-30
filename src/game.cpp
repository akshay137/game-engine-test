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
		return current_game->setup(*this, game_fbo.width, game_fbo.height);
	}

	void Game::start_game()
	{
		state = GameState::Game;
		if (current_game) current_game->reset(*this, game_fbo.width, game_fbo.height);
	}

	void Game::pause_game(bool pause)
	{
		if (pause) state = GameState::Pause;
		else state = GameState::Game;
	}

	void Game::game_over(int score)
	{
		state = GameState::EndGame;
		this->score = score;
	}

	void Game::show_menu()
	{
		auto screen = get_window_size();
		auto pen = screen_to_world({}, screen);
		pen = uber.write_format(pen, font, style, "Menu\nPress enter to play");
	}

	void Game::show_endgame()
	{
		auto screen = get_window_size();
		auto pen = screen_to_world({}, screen);
		pen = uber.write_format(pen, font, style,
			"Game Over\nScore: %d\nPress R to replay\nPress M to go back to main menu",
			score
		);
	}

	void Game::show_pausemenu()
	{
		auto screen = get_window_size();
		auto pen = screen_to_world({}, screen);
		pen = uber.write_format(pen, font, style,
			"Game paused\nPress escape again to resume\nPress M to go back to menu\n"
		);
	}

	glm::vec2 Game::get_window_size() const
	{
		return glm::vec2(ctx.main_window.width, ctx.main_window.height);
	}

	uint32_t Game::time() const
	{
		return ctx.main_clock.nanoseconds();
	}

	float Game::timef() const
	{
		return ctx.main_clock.seconds();
	}

	uhero::Result Game::load(uhero::Context& ctx)
	{
		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(16);
		style.border_size = 0.1;
		style.border_color = gfx::Color32::from_rgba(1, 0, 0);

		gfx::FBDescriptor descriptor {};
		descriptor.add_color_attachment(gfx::PixelFormat::RGBA8);
		game_fbo.create(descriptor, ctx.main_window.width, ctx.main_window.height);

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		if (current_game) current_game->cleanup();
		game_fbo.clear();
		uber.clear();
		font.clear();
	}

	void Game::update(float delta)
	{
		auto& ip = ctx.input;
		auto& audio = ctx.audio;

		// if (ip.is_key_released(KeyCode::Escape))
		// 	ctx.request_exit();
		if (ip.is_key_released(KeyCode::Tilde))
			debug_info_enabled = !debug_info_enabled;
		
		switch (state)
		{
			case GameState::Menu:
			{
				if (ip.is_key_pressed(KeyCode::Escape))
					ctx.request_exit();
				if (ip.is_key_pressed(KeyCode::Return))
					start_game();
			}
				break;
			
			case GameState::Pause:
			{
				if (ip.is_key_pressed(KeyCode::Escape))
					pause_game(false);
				if (ip.is_key_pressed(KeyCode::M))
					state = GameState::Menu;
			}
				break;
			
			case GameState::Game:
				if (current_game) current_game->update(*this, delta);
				break;
			
			case GameState::EndGame:
			{
				if (ip.is_key_pressed(KeyCode::R))
					start_game();
				if (ip.is_key_pressed(KeyCode::M))
					state = GameState::Menu;
			}
				break;
		}
	}

	void Game::render()
	{
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);
		auto center = screen * .5f;

		if (GameState::Game == state)
		{
			ctx.gfx.use_framebuffer(game_fbo);
			if (current_game) current_game->draw(*this);
			uber.flush();
			uber.draw_texture(center, screen, game_fbo.color[0],
				glm::vec4(0, game_fbo.height, game_fbo.width, -game_fbo.height)
			);
		}

		ctx.gfx.use_default_framebuffer(ctx.main_window);
		if (GameState::Menu == state)
		{
			show_menu();
		}
		else if (GameState::Game != state)
		{
			uber.draw_texture(center, screen, game_fbo.color[0],
				glm::vec4(0, game_fbo.height, game_fbo.width, -game_fbo.height)
			);
			uber.draw_color(center, screen, gfx::Color32::from_rgba(.2, .2, .2, .5));
			
			if (GameState::Pause == state)
				show_pausemenu();
			else if (GameState::EndGame == state)
				show_endgame();
		}

		// game debug info
		if (debug_info_enabled)
			show_debug_info();
		uber.flush();
	}

	void Game::show_debug_info()
	{
		auto stats = gfx::Context::gpu_stats;

		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto time = ctx.main_clock.milliseconds();
		i32 ms = time;
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