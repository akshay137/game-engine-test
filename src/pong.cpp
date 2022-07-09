#include "pong.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "uhero/logger.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/uhero.hpp"

#include <glm/glm.hpp>
#include <cmath>

namespace game
{
	using namespace uhero;
	glm::vec4 clip = { 12 * 16, 0, 16, 16 };
	gfx::Texture tx_circle;

	bool Pong::load(Game&, int width, int height)
	{
		game_size = { width, height };

		return true;
	}

	void Pong::cleanup()
	{
	}

	void Pong::reset(Game& game, int width, int height)
	{
		this->game = &game;
		game_size = { width, height };
		ball_velocity = 5 * SIM_TO_VIEW;
		pad_acceleration = 50 * SIM_TO_VIEW;

		float size = 32 + 16;
		float x = width / 2;
		float y = height / 2;
		float r = height / 2;

		playground = math::Circle({x, y}, r);
		float sr = size * 1.5;
		trap_points[0] = math::Circle({ x, height }, sr);
		trap_points[1] = math::Circle({ x, 0 }, sr);
		trap_points[2] = math::Circle({ x - r, y }, sr);
		trap_points[3] = math::Circle({ x + r, y }, sr);

		glm::vec2 tpos = glm::normalize(glm::vec2(1, 1)) * r;
		x = playground.origin.x;
		y = playground.origin.y;
		score_points[0] = math::Circle({ x + tpos.x, y + tpos.y }, sr); // top right
		score_points[1] = math::Circle({ x + tpos.x, y - tpos.y }, sr); // bottom right
		score_points[2] = math::Circle({ x - tpos.x, y + tpos.y }, sr); // top left
		score_points[3] = math::Circle({ x - tpos.x, y - tpos.y }, sr); // bottom left


		x = playground.origin.x;
		y = playground.origin.y + playground.radius - sr * 3;
		player = { math::Circle(glm::vec2(width / 2, height / 4), size), glm::vec2(0) };
		ball = { math::Circle({ x, y }, size * .5f), glm::vec2(0.0f, -ball_velocity) };
		score = 0;
	}

	void Pong::displace_ball(Ball& ball, const Ball& rhs)
	{
		if (!ball.collides(rhs)) return;
		
		auto inorm = glm::normalize(ball.position() - rhs.position());
		auto incedent = glm::normalize(ball.velocity);;
		auto direction = glm::reflect(incedent, inorm);
		ball.velocity = direction * ball_velocity;
		auto temp = inorm * (rhs.radius() + ball.radius() + 1);
		ball.circle.origin = rhs.position() + temp;
	}

	int Pong::move_ball(float delta)
	{
		ball.circle.origin += ball.velocity * delta;

		if (!ball.inside(playground))
		{
			auto inorm = glm::normalize(ball.position() - playground.origin);
			auto incedent = glm::normalize(ball.velocity);
			auto direction = glm::reflect(incedent, inorm);
			auto temp = inorm * (playground.radius - ball.radius() - 1);
			ball.circle.origin = playground.origin + temp;
			ball.velocity = direction * ball_velocity;
		}
		
		for (auto i = 0; i < MAX_SCORE_POINTS; i++)
		{
			if (ball.collides(score_points[i]))
			{
				Ball temp(score_points[i]);
				displace_ball(ball, temp);
				return 1;
			}
		}

		for (auto i = 0; i < MAX_TRAPS; i++)
		{
			if (ball.collides(trap_points[i]))
			{
				game->game_over(score);
			}
		}

		displace_ball(ball, player);
		return 0;
	}

	void Pong::update(Game& game, float delta)
	{
		auto& ip = game.ctx.input;
		if (ip.is_key_pressed(KeyCode::Escape))
		{
			game.pause_game();
			return;
		}

		glm::vec2 direction(0);
		if (ip.is_key_down(KeyCode::W))
			direction.y = 1;
		if (ip.is_key_down(KeyCode::S))
			direction.y = -1;
		if (ip.is_key_down(KeyCode::A))
			direction.x = -1;
		if (ip.is_key_down(KeyCode::D))
			direction.x = 1;
		
		auto tmp = move_ball(delta);
		score += tmp;
		
		auto pvelocity = direction * pad_acceleration * delta;
		player.velocity = glm::mix(player.velocity + pvelocity, glm::vec2(0), 2 * delta);

		player.circle.origin += player.velocity * delta;
		player.circle.origin = glm::clamp(player.circle.origin,
			glm::vec2(player.radius()), game_size - player.radius()
		);
	}

	void Pong::draw(Game& game)
	{
		auto& uber = game.uber;
		game.clear_game_screen(gfx::Color32(32, 32, 32, 255));

		uber.draw_circle(Utils::snap_to_pixel(playground.origin),
			playground.radius, gfx::WHITE
		);
		uber.draw_circle(Utils::snap_to_pixel(player.position()), player.radius(),
			gfx::RED
		);

		auto color = gfx::BLUE;
		uber.draw_circle(Utils::snap_to_pixel(ball.position()), ball.radius(), color);

		for (auto i = 0; i < MAX_SCORE_POINTS; i++)
		{
			uber.draw_circle(Utils::snap_to_pixel(score_points[i].origin),
				score_points[i].radius, gfx::GREEN.with_alpha(.6)
			);
		}
		for (auto i = 0; i < MAX_TRAPS; i++)
		{
			uber.draw_circle(Utils::snap_to_pixel(trap_points[i].origin),
				trap_points[i].radius, gfx::RED.with_alpha(.6)
			);
		}

		auto pen = game.screen_to_world( {0, 0}, game_size);
		pen = uber.write_format(pen, game.font, game.style,
			"Score: %d\n", score
		);
	}
}