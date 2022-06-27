#ifndef GAME_PONG_H__
#define GAME_PONG_H__ 1

#include "minigame.hpp"
#include <glm/vec2.hpp>

namespace game
{
	struct Pad
	{
		glm::vec2 position;
		glm::vec2 size;
		glm::vec2 velocity;

		float& x() { return position.x; }
		float& y() { return position.y; }
		float& width() { return size.x; }
		float& height() { return size.y; }

		float left() const { return position.x - size.x * .5; }
		float right() const { return position.x + size.x * .5; }
		float top() const { return position.y + size.y * .5; }
		float bottom() const { return position.y - size.y * .5; }
	};

	struct Ball
	{
		glm::vec2 position;
		float radius;
		glm::vec2 velocity;

		float& x() { return position.x; }
		float& y() { return position.y; }
		float& size() { return radius; }

		float left() const { return position.x - radius; }
		float right() const { return position.x + radius; }
		float top() const { return position.y + radius; }
		float bottom() const { return position.y - radius; }
	};

	struct Pong : public MiniGame
	{
		Pad player, comp;
		Ball ball;
		int game_width, game_height;

		float MAX_BALL_SPEED;
		float bat_speed;
		float ball_speed;
		float damp;

		int score;

		bool check_ball_collision(const Pad& pad) const;
		// return value to be added to score
		int move_ball(float);
		void move_comp(float);

		bool setup(Game&, int, int) override;
		void update(Game&, float) override;
		void draw(Game&) override;

		void cleanup() override;
	};
}

#endif