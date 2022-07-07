#ifndef GAME_H__
#define GAME_H__ 1

#include "uhero/application.hpp"
#include "uhero/gfx/renderer.hpp"
#include "uhero/gfx/font.hpp"
#include "uhero/gfx/framebuffer.hpp"
#include "uhero/sfx/sfx.hpp"
#include "uhero/math/rectangle.hpp"
#include "minigame.hpp"
#include "colorswitch.hpp"
#include "pong.hpp"

#include <string_view>
#include <glm/vec2.hpp>

namespace game
{
	enum class GameState : int
	{
		Menu, // shows list of minigames
		Game, // gameplay
		EndGame, // win, loose, etc.
		Pause, // pause menu
	};

	// standard button
	struct Button
	{
		uhero::math::Rectangle rect;
		std::string_view title;

		bool is_over(glm::vec2 mouse) const
		{
			return rect.is_point_inside(mouse);
		}
	};

	struct Game : public uhero::IApplication
	{
		uhero::Context& ctx;
		uhero::gfx::Renderer uber;
		uhero::gfx::Font font;
		uhero::gfx::FontStyle style;
		uhero::gfx::FrameBuffer game_fbo;

		uhero::sfx::AudioBuffer s_bgm;
		uhero::sfx::AudioBuffer s_click;
		float volume_bgm;
		float volume_click;

		ColorSwitch color_switch;
		Pong pong;
		
		// main_menu
		Button btn_games[2];
		Button btn_exit;

		Button btn_resume; // pause
		Button btn_restart; // endgame
		Button btn_menu; // pause | endgame

		GameState state;
		MiniGame* current_game;
		int score;

		bool debug_info_enabled = true;

		Game(uhero::Context& ctx)
			: ctx{ctx}, state{GameState::Menu}, current_game{nullptr}
		{}

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

		bool set_minigame(MiniGame* mgame);

		void start_game();
		void pause_game(bool pause=true);
		void game_over(int score);
		void show_menu();
		void show_endgame();
		void show_pausemenu();

		void draw_button(const Button& button);

		glm::vec2 screen_to_world(glm::vec2 pos, glm::vec2 screen)
		{
			return glm::vec2(pos.x, screen.y - pos.y);
		}

		void clear_game_screen(uhero::gfx::Color32 color)
		{
			game_fbo.clear_buffers(color, 1, 0);
		}

		glm::vec2 get_window_size() const;
		glm::vec2 get_game_size() const;
		uint32_t time() const;
		float timef() const;

		// debug
		void show_debug_info();
	};
}

#endif