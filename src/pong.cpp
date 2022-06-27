#include "pong.hpp"
#include "game.hpp"
#include "uhero/uhero.hpp"

#include <cmath>

namespace game
{
	using namespace uhero;
	const gfx::Color32 COLOR_RED(255, 0, 0, 255);
	const gfx::Color32 COLOR_GREEN(0, 255, 0, 255);
	const gfx::Color32 COLOR_BLUE(0, 0, 255, 255);

	glm::vec2 snap_to_pixel(const glm::vec2& pos)
	{
		return { round(pos.x), round(pos.y) };
	}

	template <typename T>
	T lerp(const T& from, const T& to, float time)
	{
		return from + (to - from) * time;
	}

	template <typename T>
	T clamp(const T& value, const T& lower, const T& upper)
	{
		return std::min(upper, std::max(value, lower));
	}

	bool Pong::check_ball_collision(const Pad& pad) const
	{
		if (ball.left() > pad.right()) return false;
		if (ball.right() < pad.left()) return false;
		if (ball.top() < pad.bottom()) return false;
		if (ball.bottom() > pad.top()) return false;
		return true;
	}

	int Pong::move_ball(float delta)
	{
		ball.position += ball.velocity * delta;

		// out of bounds
		if (ball.left() < 0)
		{
			ball.velocity.x = ball_speed;
			return -1;
		}
		if (ball.right() > game_width)
		{
			ball.velocity.x = -ball_speed;
			return 1;
		}
		if (ball.top() > game_height)
		{
			ball.velocity.y = -ball_speed;
		}
		else if (ball.bottom() < 0)
		{
			ball.velocity.y = ball_speed;
		}

		if (check_ball_collision(player))
		{
			ball.position.x = player.right() + ball.radius;
			float ratio = (ball.y() - player.y()) / player.height();
			ball.velocity.y = ball_speed * ratio;
			ball.velocity.x *= -1;
			return 1;
		}
		else if (check_ball_collision(comp))
		{
			ball.position.x = comp.left() - ball.radius;
			ball.velocity.x *= -1;
			float ratio = (ball.y() - comp.y()) / comp.height();
			ball.velocity.y = ball_speed * ratio;
			return 0;
		}

		return 0;
	}

	void Pong::move_comp(float delta)
	{
		// TODO
	}

	bool Pong::setup(Game& game, int width, int height)
	{
		game_width = width;
		game_height = height;

		MAX_BALL_SPEED = 100;
		bat_speed = 150;
		ball_speed = 250;
		damp = 10;

		glm::vec2 bat_size(16, 16 * 8);
		player = { glm::vec2(32, height / 2), bat_size, glm::vec2(0)};
		comp = { glm::vec2(width - 32, height / 2), bat_size, glm::vec2(0) };
		ball = { glm::vec2(width / 2, height / 2), 8, glm::vec2(ball_speed) };
	}

	void Pong::cleanup()
	{}

	void Pong::update(Game& game, float delta)
	{
		auto& ip = game.ctx.input;

		glm::vec2 direction(0);
		if (ip.is_key_down(KeyCode::W))
			direction.y = 1;
		if (ip.is_key_down(KeyCode::S))
			direction.y = -1;
		
		float upper = game_height - player.size.y * .5;
		float lower = player.size.y * .5;
		
		auto pvelocity = direction * bat_speed * delta;
		player.velocity = lerp(player.velocity + pvelocity, glm::vec2(0), damp * delta);

		player.position += player.velocity;
		player.position.y = clamp(player.position.y, lower, upper);

		auto tmp = move_ball(delta);
	}

	void Pong::draw(Game& game)
	{
		auto& uber = game.uber;

		uber.draw_color(snap_to_pixel(player.position),
			player.size, COLOR_RED
		);
		uber.draw_color(snap_to_pixel(comp.position),
			comp.size, COLOR_GREEN
		);
		uber.draw_color(snap_to_pixel(ball.position),
			glm::vec2(ball.radius * 2), COLOR_BLUE
		);

		auto pen = game.screen_to_world( {0, 0}, { game_width, game_height });
		pen = uber.write_format(pen, game.font, game.style, "player: [%.2f, %.2f]\n", player.position.x, player.position.y);
		pen = uber.write_format(pen, game.font, game.style, "comp: [%.2f, %.2f]\n", comp.position.x, comp.position.y);
		pen = uber.write_format(pen, game.font, game.style, "ball: [%.2f, %.2f]\n", ball.position.x, ball.position.y);
	}
}