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
		auto size = get_window_size();
		uber.draw_color(size * .5f, size, gfx::Color32(32));

		draw_button(btn_games[0]);
		draw_button(btn_games[1]);
		draw_button(btn_exit);

		auto pen = screen_to_world(glm::vec2(128, 64), size);
		auto _style = style;
		_style.text_color = gfx::Color32(255, 74, 92);
		_style.border_color = gfx::Color32(0, 255, 255);
		_style.border_size = .05;
		_style.set_size(128);
		pen = uber.write_format(pen, font, _style, "unsignedHERO\n");

		_style.set_size(128 - 32);
		_style.text_color = gfx::Color32(255);
		_style.border_size = 0;
		pen = uber.write_format(pen, font, _style, "game engine\n");
	}

	void Game::show_endgame()
	{
		draw_button(btn_restart);
		draw_button(btn_menu);

		auto screen = get_window_size();
		auto pen = screen_to_world({ screen.x / 2, 0 }, screen);
		pen.x -= 512;
		auto _style = style;
		_style.set_size(128);
		_style.text_color = gfx::Color32::from_rgba(1, .2, .3);
		_style.border_color = gfx::Color32(255);
		_style.border_size = .025;
		pen = uber.write_format(pen, font, _style,
			"Game Over\n"
		);

		_style.set_size(64);
		_style.text_color = gfx::Color32::from_rgba(0, 1, 0);
		_style.border_size = .05;
		pen.x += 256;
		pen = uber.write_format(pen, font, _style, "Score: %d\n", score);
	}

	void Game::show_pausemenu()
	{
		draw_button(btn_resume);
		draw_button(btn_menu);

		auto screen = get_window_size();
		auto pen = screen_to_world(screen * .5f, screen);
		auto _style = style;
		_style.set_size(64);
		pen.x -= (256 + 32);
		pen.y += 256;
		pen = uber.write_format(pen, font, _style, "Game paused\n");
	}

	void Game::draw_button(const Button& button)
	{
		auto pos = button.rect.position;
		auto size = button.rect.size;
		uber.draw_color(pos, size, gfx::Color32::from_rgba(1, .4, .5));
		size -= glm::vec2(8);
		uber.draw_color(pos, size, gfx::Color32(255));
		size -= glm::vec2(4);
		uber.draw_color(pos, size, gfx::Color32(0));

		auto _style = style;
		_style.border_color = gfx::Color32(31);
		_style.text_color = gfx::Color32(255);
		_style.border_size = .1;
		glm::vec2 box;
		font.get_box_size(button.title.size(), button.title.data(),
			style, box.x, box.y
		);
		if (box.x > button.rect.width())
		{
			auto scale = ((button.rect.width() * .95) / box.x);
			auto pt = (style.size / gfx::PT_TO_PIXEL) * scale;
			_style.set_size(pt);
			_style.border_size *= scale * 2;
			box *= (_style.size / style.size);
		}
		glm::vec2 pen = { button.rect.left(), button.rect.top() };
		pen.x += button.rect.size.x * .5f - box.x * .5f;
		pen.x += _style.size * .5f;
		pen.y -= button.rect.size.y * .5f - box.y;
		pen = uber.write(pen, font, _style, button.title.size(), button.title.data());
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

	uhero::Result Game::load(uhero::Context&)
	{
		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(32);
		style.border_size = 0.05;
		style.border_color = gfx::Color32::from_rgba(0, 0, 0);

		gfx::FBDescriptor descriptor {};
		descriptor.add_color_attachment(gfx::PixelFormat::RGBA8);
		game_fbo.create(descriptor, 1600, 900);

		auto screen = get_window_size();
		auto pos = screen * .5f;
		glm::vec2 btn_size = { 512, 64 + 32 };
		
		pos.y = pos.y - btn_size.y * 1.5;
		btn_games[0] = { Rectangle(pos, btn_size), "Pong [P]" };
		pos.y -= btn_size.y * 1.2;
		btn_games[1] = { Rectangle(pos, btn_size), "Color Switch [C]" };
		pos.y -= btn_size.y * 1.2;
		btn_exit = { Rectangle(pos, btn_size), "Exit [E]" };

		pos = { screen.x / 2, 128 };
		btn_menu = { Rectangle(pos, btn_size), "Menu [M]" };
		
		pos.y += btn_size.y * 1.2;
		btn_resume = { Rectangle(pos, btn_size), "Resume [R]" };
		btn_restart = { Rectangle(pos, btn_size), "Restart [R]" };

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
		auto screen = get_window_size();

		if (ip.is_key_released(KeyCode::Tilde))
			debug_info_enabled = !debug_info_enabled;
		
		glm::vec2 mouse = { ip.mouse.x, ip.mouse.y };
		auto pointer_world = screen_to_world(mouse, screen);

		switch (state)
		{
			case GameState::Menu:
			{
				auto should_exit = ip.is_key_pressed(KeyCode::Escape);
				should_exit = should_exit || ip.is_key_pressed(KeyCode::E);
				if (should_exit)
					ctx.request_exit();
				if (ip.is_key_released(KeyCode::P)) // pong
				{
					set_minigame(&pong);
					start_game();
					return;
				}
				if (ip.is_key_released(KeyCode::C)) // color switch
				{
					set_minigame(&color_switch);
					start_game();
				}
				
				if (ip.is_mbutton_released(MouseKey::Left))
				{
					if (btn_games[0].is_over(pointer_world))
					{
						set_minigame(&pong);
						start_game();
						return;
					}
					if (btn_games[1].is_over(pointer_world))
					{
						set_minigame(&color_switch);
						start_game();
						return;
					}
					else if (btn_exit.is_over(pointer_world))
						ctx.request_exit();
				}
			}
				break;
			
			case GameState::Pause:
			{
				if (ip.is_key_pressed(KeyCode::Escape) || ip.is_key_pressed(KeyCode::R))
					pause_game(false);
				if (ip.is_key_pressed(KeyCode::M))
					state = GameState::Menu;
				if (ip.is_mbutton_released(MouseKey::Left))
				{
					if (btn_resume.is_over(pointer_world))
						pause_game(false);
					if (btn_menu.is_over(pointer_world))
						state = GameState::Menu;
				}
			}
				break;
			
			case GameState::Game:
				if (current_game) current_game->update(*this, delta);
				break;
			
			case GameState::EndGame:
			{
				if (ip.is_key_pressed(KeyCode::R))
					start_game();
				if (ip.is_key_pressed(KeyCode::M) || ip.is_key_pressed(KeyCode::Escape))
					state = GameState::Menu;
				if (ip.is_mbutton_released(MouseKey::Left))
				{
					if (btn_restart.is_over(pointer_world))
						start_game();
					if (btn_menu.is_over(pointer_world))
						state = GameState::Menu;
				}
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
		ctx.gfx.clear_buffer(gfx::Color32(0), 1, 0);
		if (GameState::Menu == state)
		{
			show_menu();
		}
		else if (GameState::Game != state)
		{
			uber.draw_texture(center, screen, game_fbo.color[0],
				glm::vec4(0, game_fbo.height, game_fbo.width, -game_fbo.height)
			);
			uber.draw_color(center, screen, gfx::Color32(31, 255 - 32));
			
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
		uber.flush();
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
		auto _style = gfx::FontStyle(15);
		_style.border_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_size = 0.125;
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