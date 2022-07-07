#ifndef GAME_PONG_H__
#define GAME_PONG_H__ 1

#include "minigame.hpp"
#include "ball.hpp"
#include "uhero/gfx/texture.hpp"
#include "uhero/gfx/color.hpp"
#include <glm/vec2.hpp>

#include "gameobject.hpp"
#include "camera.hpp"

namespace game
{
	struct Sprite : public GObject
	{
		uhero::gfx::Color32 color;
		const uhero::gfx::Texture* texture;
		glm::vec4 clip;
		glm::vec2 size;

		Sprite(): color{255}, texture{nullptr}, clip{0}, size{0} {}

		Sprite(const uhero::gfx::Texture& tex)
			: color{255}, texture{&tex}, clip{0, 0, tex.width, tex.height},
				size{tex.width, tex.height}
		{}

		operator const uhero::gfx::Texture& () const
		{
			return *texture;
		}
	};

	constexpr int MAX_SCORE_POINTS = 4;
	constexpr int MAX_TRAPS = 4;
	struct Pong : public MiniGame
	{
		Game* game;
		glm::vec2 game_size; // rectangular size
		Circle playground;
		Ball player, ball;
		Circle score_points[MAX_SCORE_POINTS];
		Circle trap_points[MAX_TRAPS];

		Camera2D camera;
		Sprite test_sprite;

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