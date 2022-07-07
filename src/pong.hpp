#ifndef GAME_PONG_H__
#define GAME_PONG_H__ 1

#include "minigame.hpp"
#include "ball.hpp"
#include "uhero/gfx/texture.hpp"
#include "uhero/gfx/color.hpp"
#include <glm/vec2.hpp>

#include "gameobject.hpp"
#include "sprite.hpp"
#include "camera.hpp"

namespace game
{
	constexpr int MAX_SCORE_POINTS = 4;
	constexpr int MAX_TRAPS = 4;
	struct Pong : public MiniGame
	{
		Game* game;
		glm::vec2 game_size; // rectangular size
		uhero::math::Circle playground;
		Ball player, ball;
		uhero::math::Circle score_points[MAX_SCORE_POINTS];
		uhero::math::Circle trap_points[MAX_TRAPS];

		Camera2D camera;
		Sprite test_sprite;
		// Sprite player, ball;
		// Sprite ground;
		// Sprite score_pt[MAX_SCORE_POINTS];
		// Sprite trap_pt[MAX_TRAPS];

		constexpr static float SIM_TO_VIEW = 50;
		float pad_acceleration;
		float ball_velocity;
		
		int score;

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