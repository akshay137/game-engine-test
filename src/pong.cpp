#include "pong.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/uhero.hpp"

#include <glm/glm.hpp>
#include <cmath>

namespace game
{
	using namespace uhero;
	const gfx::Color32 COLOR_WHITE(255);
	const gfx::Color32 COLOR_RED(255, 0, 0, 255);
	const gfx::Color32 COLOR_GREEN(0, 255, 0, 255);
	const gfx::Color32 COLOR_BLUE(0, 0, 255, 255);

	glm::vec4 clip = { 12 * 16, 0, 16, 16 };
	gfx::Texture tx_circle;

	bool Pong::load(Game&, int width, int height)
	{
		game_width = width;
		game_height = height;

		tx_circle = res::load_texture("assets/logo.png");
		tx_circle.set_filter(gfx::TextureFilter::Nearest);

		return true;
	}

	void Pong::cleanup()
	{
		tx_circle.clear();
	}

	void Pong::reset(Game&, int width, int height)
	{
		MAX_BALL_SPEED = 100;
		bat_speed = 200;
		ball_speed = 250;
		damp = 10;

		score = 0;
		score_multiplier = 1;

		float size = 32 + 16;
		float margin = size + (size / 1.5);
		ground = Circle({width / 2, height / 2}, game_height / 2);
		player = { Circle(glm::vec2(margin, height / 2), size), glm::vec2(0) };
		comp = { Circle(glm::vec2(width - margin, height / 2), size), glm::vec2(0) };
		ball = { Circle(ground.origin, size * .5f), glm::vec2(ball_speed) };
	}

	void Pong::displace_ball(Ball& ball, const Ball& rhs)
	{
		if (!ball.collides(rhs)) return;
		
		auto inorm = glm::normalize(ball.position() - rhs.position());
		auto incedent = glm::normalize(ball.velocity);
		auto direction = glm::reflect(incedent, inorm);
		ball.velocity += rhs.velocity + direction * ball_speed;
		auto temp = inorm * (rhs.radius() + ball.radius());
		ball.circle.origin = rhs.position() + temp;
	}

	int Pong::move_ball(float delta)
	{
		ball.circle.origin += ball.velocity * delta;

		if (ball.top() > game_height)
			ball.velocity.y *= -1;
		else if (ball.bottom() < 0)
			ball.velocity.y *= -1;
		if (ball.left() < 0)
			ball.velocity.x *= -1;
		if (ball.right() > game_width)
			ball.velocity.x *= -1;
		
		displace_ball(ball, player);
		displace_ball(ball, comp);

		ball.circle.origin = glm::clamp(ball.position(),
			glm::vec2(ball.radius()),
			glm::vec2(game_width, game_height) - ball.radius()
		);

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
		
		float upper = game_height - player.size().y * .5;
		float lower = player.size().y * .5;
		
		auto pvelocity = direction * bat_speed;
		glm::vec2 max_vel(bat_speed * 10);
		player.velocity = glm::mix(player.velocity + pvelocity, glm::vec2(0), damp * delta);
		player.velocity = glm::clamp(player.velocity, -max_vel, max_vel);

		player.circle.origin += player.velocity * delta;
		player.circle.origin.y = glm::clamp(player.position().y, lower, upper);

		auto tmp = move_ball(delta);
		score += tmp * score_multiplier;
	}

	void Pong::draw(Game& game)
	{
		auto& uber = game.uber;
		game.clear_game_screen(gfx::Color32(32, 32, 32, 255));

		auto direction = glm::normalize(ball.position() - player.position());
		float angle = glm::atan(direction.y, direction.x);

		uber.draw_texture(Utils::snap_to_pixel(ground.origin),
			ground.quad_size(), tx_circle, angle, glm::vec2(0), 1
		);
		uber.draw_circle(Utils::snap_to_pixel(player.position()), player.radius(), COLOR_RED);
		uber.draw_circle(Utils::snap_to_pixel(comp.position()), comp.radius(), COLOR_GREEN);

		auto color = COLOR_BLUE;
		if (ball.collides(ground))
			color = gfx::Color32(255, 255, 0, 255);
		if (ball.inside(ground))
			color = gfx::Color32(0, 255, 255, 255);
		uber.draw_circle(Utils::snap_to_pixel(ball.position()), ball.radius(), color);

		auto pen = game.screen_to_world( {0, 0}, { game_width, game_height });
		pen = uber.write_format(pen, game.font, game.style,
			"Score: %d (x%d)\n", score, score_multiplier
		);
		
		// debug
		pen = uber.write_format(pen, game.font, game.style, "player: [%.2f, %.2f]\n", player.position().x, player.position().y);
		pen = uber.write_format(pen, game.font, game.style, "comp: [%.2f, %.2f]\n", comp.position().x, comp.position().y);
		pen = uber.write_format(pen, game.font, game.style, "ball: [%.2f, %.2f]\n", ball.position().x, ball.position().y);
		pen = uber.write_format(pen, game.font, game.style, "angle: %f\n", glm::degrees(angle));
	}
}