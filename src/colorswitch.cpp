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
		ball.circle.origin = game_size * .5f;
		ball.velocity = { 0, 0 };
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
		auto index = rand() % MAX_COLORS;
		pad.first_color = index;
		index = rand() % pad.segment_count;
		ball_color_index = (pad.first_color + index) % MAX_COLORS;
	}

	void ColorSwitch::reset(Game&, int width, int height)
	{
		game_size = { width, height };

		score = 0;

		ball = { Circle({ width / 2, height / 2 }, 24), glm::vec2(0) };
		pad = ColorPad(glm::vec2(width / 2, 128), ball.radius() * 6);
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

		ball_color_index = pad.first_color + pad.segment_count / 2;
	}

	void ColorSwitch::update(Game& game, float delta)
	{
		auto& ip = game.ctx.input;

		if (ip.is_key_released(KeyCode::NP_PLUS))
		{
			pad.resegment(pad.segment_count + 1);
		}
		if (ip.is_key_released(KeyCode::NP_MINUS))
		{
			pad.resegment(pad.segment_count - 1);
		}

		float MOVE_SPEED = 200;
		if (ip.is_key_down(KeyCode::A)) // left
		{
			pad.velocity.x -= MOVE_SPEED;
		}
		if (ip.is_key_down(KeyCode::D)) // right
		{
			pad.velocity.x += MOVE_SPEED;
		}
		pad.rect.position += pad.velocity * delta;
		pad.velocity = glm::mix(pad.velocity, glm::vec2(0), 5 * delta);

		ball.velocity += gravity * delta;
		ball.circle.origin += ball.velocity * delta;

		if (ball.bottom() < 0)
		{
			reset_ball();
		}

		if (check_ball_pad_collision())
		{
			const auto index = pad.get_color_index_at(ball.position()) % MAX_COLORS;
			if (index == ball_color_index)
			{
				++score;
				ball.velocity = gravity * -1.25f;
				switch_color();
			}
			else
			{
				// reset(game, game_size.x, game_size.y);
				reset_ball();
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

		// debug
		auto& font = game.font;
		auto& style = game.style;
		auto pen = game.screen_to_world(glm::vec2(0), game.get_window_size());
		pen = uber.write_format(pen, font,style, "Score: %d\n", score);
		pen = uber.write_format(pen, font, style,
			"ball: [%.2f, %.2f] | c: %d\n",
			ball.position().x, ball.position().y,
			ball_color_index
		);
		auto cindex = pad.get_color_index_at(ball.position());
		pen = uber.write_format(pen, font, style,
			"color: %d\n", cindex
		);
		pen = uber.write_format(pen, font, style,
			"cpad: [%.2f, %.2f] { %.2f, %.2f } | s: %d | f: %d\n",
			pad.rect.position.x, pad.rect.position.y,
			pad.rect.size.x, pad.rect.size.y,
			pad.segment_count, pad.first_color
		);
		for (auto i = 0; i < pad.segment_count; i++)
		{
			glm::vec2 pos = pad.segment_position(i);
			glm::vec2 size = pad.segment_size();
			int color_index = pad.segment_color_index(i) % MAX_COLORS;

			pen = uber.write_format(pen, font, style,
				"-segment_%d: [%.2f, %.2f] { %.2f, %.2f } | color: %8X\n",
				i, pos.x, pos.y, size.x, size.y,
				colors[color_index].to_rgba_u32()
			);
		}
	}
}