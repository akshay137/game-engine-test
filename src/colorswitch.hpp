#ifndef GAME_COLORSWITCH_H__
#define GAME_COLORSWITCH_H__ 1

#include "minigame.hpp"
#include "ball.hpp"
#include "colorpad.hpp"
#include "uhero/gfx/color.hpp"

namespace game
{
	struct ColorSwitch : public MiniGame
	{
		glm::vec2 game_size;
		constexpr static int MAX_COLORS = 8;
		uhero::gfx::Color32 colors[MAX_COLORS];

		int score;
		int ball_color_index;
		Ball ball;
		ColorPad pad;

		glm::vec2 gravity;

		void reset_ball();
		bool check_ball_pad_collision();
		void switch_color();

		bool load(Game&, int, int) override;
		void cleanup() override;

		void reset(Game&, int, int) override;
		
		void update(Game&, float) override;
		void draw(Game&) override;
	};
}

#endif