#include "colorswitch.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "uhero/uhero.hpp"
#include "uhero/res/audio.hpp"

#include <glm/glm.hpp>

float updt;

namespace game
{
	using namespace uhero;

	constexpr float SIM_TO_VIEW_RATIO = 50;

	bool ColorSwitch::load(Game&, int, int)
	{
		s_hit = res::load_audio("assets/hit.wav");
		volume_hit = .3;
		return true;
	}

	void ColorSwitch::cleanup()
	{
		s_hit.clear();
	}

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
		pad = ColorPad(glm::vec2(width / 2, height * 0.15), ball.radius() * 7);
		pad.resegment(3);
		pad.first_color = 1;

		colors[0] = gfx::Color32::from_rgba(1, 0, 0);
		colors[1] = gfx::Color32::from_rgba(0, 1, 0);
		colors[2] = gfx::Color32::from_rgba(0, 0, 1);
		colors[3] = gfx::Color32::from_rgba(0, 1, 1);
		colors[4] = gfx::Color32::from_rgba(1, 0, 1);
		colors[5] = gfx::Color32::from_rgba(1, 1, 0);

		gravity = glm::vec2(0.0f, -9.8f * SIM_TO_VIEW_RATIO);
		switch_color();
	}

	void ColorSwitch::update(Game& game, float delta)
	{
		updt = delta;
		if (check_ball_pad_collision())
		{
			const auto index = pad.get_color_index_at(ball.position()) % MAX_COLORS;
			ball.circle.origin.y = pad.top() + ball.radius();
			glm::vec2 tmp = {
				pad.velocity.x + ball.velocity.x,
				gravity.y * -1.25f
			};
			ball.velocity = tmp;
			float limit = 128;
			ball.velocity.x = glm::clamp(ball.velocity.x, -limit, limit);

			if (index == ball_color_index)
			{
				game.ctx.audio.play_buffer(s_hit, volume_hit);
				++score;
				if (0 == score % 5)
				{
					auto seg = random_generator.range(2, MAX_COLORS);
					pad.resegment(seg);
				}
				switch_color();
			}
			else
			{
				game.game_over(score);
				return;
			}
		}

		auto& ip = game.ctx.input;
		if (ip.is_key_pressed(KeyCode::Escape))
		{
			game.pause_game(true);
			return;
		}

		constexpr float ACCELERATION = 9.8f * 3 * SIM_TO_VIEW_RATIO;
		int direction = 0;
		if (ip.is_key_down(KeyCode::A)) // left
			direction = -1;
		if (ip.is_key_down(KeyCode::D)) // right
			direction = 1;
		
		pad.velocity = glm::mix(pad.velocity, glm::vec2(0), 2 * delta);
		glm::vec2 tmp = glm::vec2(direction, 0) * ACCELERATION;
		pad.velocity += tmp * delta;
		pad.rect.position += pad.velocity * delta;

		// prevent pad from going out of bounds
		auto size = pad.segment_size();
		float lim = pad.rect.width() * .5f - size.x;
		pad.rect.position.x = glm::clamp(pad.rect.position.x,
			-lim, game_size.x + lim
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
	}

	void ColorSwitch::draw(Game& game)
	{
		auto& uber = game.uber;
		game.clear_game_screen(gfx::Color32(32));

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
		
		auto pen = game.screen_to_world(glm::vec2(64), game_size);
		gfx::FontStyle _style(48);
		_style.border_color = gfx::Color32(255, 0, 0);
		_style.border_size = .025;
		pen = uber.write_format(pen, game.font, _style, "Score: %d\n", score);

		// pen = uber.write_format(pen, game.font, _style, "%.2f, %.2f {%f}| %f\n",
		// 	pad.velocity.x, pad.velocity.y, pad.velocity.x * updt, updt
		// );
	}
}