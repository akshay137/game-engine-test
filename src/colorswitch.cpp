#include "colorswitch.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "uhero/uhero.hpp"

#include <glm/glm.hpp>

namespace game
{
	using namespace uhero;

	bool ColorSwitch::load(Game&, int, int)
	{
		return true;
	}

	void ColorSwitch::cleanup()
	{}

	void ColorSwitch::reset_ball()
	{
		ball = { Circle(game_size * .5f, 24), {0, 0}};
	}

	bool ColorSwitch::check_ball_pad_collision()
	{
		if (pad.right() < ball.left()) return false;
		if (pad.top() < ball.bottom()) return false;
		if (pad.left() > ball.right()) return false;
		if (pad.bottom() > ball.top()) return false;
		return true;
	}

	void ColorSwitch::switch_color()
	{
		auto index = random_generator.range(0, MAX_COLORS);
		pad.first_color = index;
		index = random_generator.range(0, pad.segment_count);
		ball_color_index = (pad.first_color + index) % MAX_COLORS;

		// shuffle color list
		auto half = MAX_COLORS / 2;
		for (auto i = 0; i < MAX_COLORS / 2; i++)
		{
			auto src = random_generator.range(0, half);
			auto dst = random_generator.range(half, MAX_COLORS);
			std::swap(colors[src], colors[dst]);
		}
	}

	void ColorSwitch::reset(Game& game, int width, int height)
	{
		random_generator = math::Random(game.time());
		game_size = { width, height };

		score = 0;

		reset_ball();
		pad = ColorPad(glm::vec2(width / 2, 128), ball.radius() * 7);
		pad.resegment(3);
		pad.first_color = 1;

		colors[0] = gfx::Color32(255);
		colors[1] = gfx::Color32::from_rgba(1, 0, 0);
		colors[2] = gfx::Color32::from_rgba(0, 1, 0);
		colors[3] = gfx::Color32::from_rgba(0, 0, 1);
		colors[4] = gfx::Color32::from_rgba(0, 1, 1);
		colors[5] = gfx::Color32::from_rgba(1, 0, 1);
		colors[6] = gfx::Color32::from_rgba(1, 1, 0);
		colors[7] = gfx::Color32::from_rgba(.1, .1, .1);

		gravity = glm::vec2(0.0f, -9.8f * 50);
		switch_color();
	}

	void ColorSwitch::update(Game& game, float delta)
	{
		auto& ip = game.ctx.input;
		if (ip.is_key_pressed(KeyCode::Escape))
		{
			game.pause_game(true);
			return;
		}

		constexpr float MOVE_SPEED = 200;
		int direction = 0;
		if (ip.is_key_down(KeyCode::A)) // left
			direction = -1;
		if (ip.is_key_down(KeyCode::D)) // right
			direction = 1;
		
		auto size = pad.segment_size();
		glm::vec2 tmp = glm::vec2(direction, 0) * MOVE_SPEED;
		pad.velocity = glm::mix(pad.velocity + tmp, glm::vec2(0), 5 * delta);
		pad.rect.position += pad.velocity * delta;

		// prevent pad from going out of bounds
		float lim = size.x * .5f * -(pad.segment_count - 1) + size.x * .5;
		pad.rect.position.x = glm::clamp(pad.rect.position.x,
			lim, game_size.x - lim
		);

		ball.velocity += gravity * delta;
		ball.circle.origin += ball.velocity * delta;
		if (ball.left() < 0 || ball.right() > game_size.x)
			ball.velocity.x *= -1.0f;

		if (ball.bottom() < 0)
		{
			game.game_over(score);
			return;
		}

		if (check_ball_pad_collision())
		{
			const auto index = pad.get_color_index_at(ball.position()) % MAX_COLORS;
			if (index == ball_color_index)
			{
				++score;
				glm::vec2 tmp = {
					pad.velocity.x + ball.velocity.x,
					gravity.y * -1.25f
				};
				ball.velocity = tmp;
				float limit = 128;
				ball.velocity.x = glm::clamp(ball.velocity.x, -limit, limit);
				if (0 == score % 5)
				{
					auto seg = random_generator.range(2, MAX_COLORS);
					pad.resegment(seg);
				}
				switch_color();
				ball.circle.origin.y = pad.top() + ball.radius();
			}
			else
			{
				game.game_over(score);
				return;
			}
		}
	}

	void ColorSwitch::draw(Game& game)
	{
		auto& uber = game.uber;

		uber.draw_circle(Utils::snap_to_pixel(ball.position()), ball.radius(),
			colors[ball_color_index]
		);

		for (auto i = 0; i < pad.segment_count; i++)
		{
			glm::vec2 pos = pad.segment_position(i);
			glm::vec2 size = pad.segment_size();
			int color_index = pad.segment_color_index(i) % MAX_COLORS;
			float angle = 0.0f;
			uber.draw_color(pos, size, colors[color_index], angle);
		}
		
		auto pen = game.screen_to_world(glm::vec2(64), game.get_window_size());
		gfx::FontStyle _style(32);
		pen = uber.write_format(pen, game.font, _style, "Score: %d\n", score);


		// debug
		// auto& font = game.font;
		// auto& style = game.style;
		// pen = uber.write_format(pen, font, style,
		// 	"ball: [%.2f, %.2f] | c: %d\n",
		// 	ball.position().x, ball.position().y,
		// 	ball_color_index
		// );
		// auto cindex = pad.get_color_index_at(ball.position());
		// pen = uber.write_format(pen, font, style,
		// 	"color: %d\n", cindex
		// );
		// pen = uber.write_format(pen, font, style,
		// 	"cpad: [%.2f, %.2f] { %.2f, %.2f } | <%.2f, %.2f> | s: %d | f: %d\n",
		// 	pad.rect.position.x, pad.rect.position.y,
		// 	pad.rect.size.x, pad.rect.size.y,
		// 	pad.left(), pad.right(),
		// 	pad.segment_count, pad.first_color
		// );
		// for (auto i = 0; i < pad.segment_count; i++)
		// {
		// 	glm::vec2 pos = pad.segment_position(i);
		// 	glm::vec2 size = pad.segment_size();
		// 	int color_index = pad.segment_color_index(i) % MAX_COLORS;

		// 	pen = uber.write_format(pen, font, style,
		// 		"-segment_%d: [%.2f, %.2f] { %.2f, %.2f } | color: %8X\n",
		// 		i, pos.x, pos.y, size.x, size.y,
		// 		colors[color_index].to_rgba_u32()
		// 	);
		// }
	}
}