#ifndef GAME_PONG_H__
#define GAME_PONG_H__ 1

#include "minigame.hpp"
#include "ball.hpp"
#include <glm/vec2.hpp>

namespace game
{
	struct Pong : public MiniGame
	{
		Circle ground;
		Ball player, comp;
		Ball ball;
		int game_width, game_height;

		float MAX_BALL_SPEED;
		float bat_speed;
		float ball_speed;
		float damp;

		int score;
		int score_multiplier;

		void displace_ball(Ball& ball, const Ball& rhs);
		int move_ball(float);

		bool load(Game&, int, int) override;
		void cleanup() override;

		void reset(Game&, int, int) override;
		void update(Game&, float) override;
		void draw(Game&) override;
	};
}

#endif