#include "test.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"
#include "uhero/math/random.hpp"

#include <glm/glm.hpp>

using namespace uhero;

namespace test
{
	Result Test::load(Context& ctx)
	{
		this->ctx = &ctx;

		renderer.create(1024 * 16);
		texture = res::load_texture("assets/logo.png");

		font = res::load_font("assets/firacode.atlas");

		camera = { ctx.main_window.width, ctx.main_window.height };
		camera *= .5f;
		return Result::Success;
	}

	void Test::clear()
	{
		renderer.clear();
		font.clear();
		texture.clear();
	}

	void Test::update(float delta)
	{
		auto& ip = ctx->input;

		if (ip.is_key_pressed(KeyCode::Escape))
		{
			ctx->request_exit();
		}

		float CAM_SPEED = 500;
		if (ip.is_key_pressed(KeyCode::R)) // reset camera
		{
			camera = { ctx->main_window.width, ctx->main_window.height };
			camera *= .5f;
		}

		if (ip.is_key_down(KeyCode::W)) // up
			camera.y += CAM_SPEED * delta;
		if (ip.is_key_down(KeyCode::S))
			camera.y -= CAM_SPEED * delta;
		
		if (ip.is_key_down(KeyCode::A))
			camera.x -= CAM_SPEED * delta;
		if (ip.is_key_down(KeyCode::D))
			camera.x += CAM_SPEED * delta;
	}

	void Test::render()
	{
		// draw colored quads
		glm::vec2 size(64);
		auto num = 64;
		auto len = num * num;
		for (auto i = 1; i < len; i += 2)
		{
			auto col = i % num;
			auto row = i / num;
			glm::vec2 pos = { col * size.x, row * size.y };
			gfx::Color32 color(255);
			auto v = i * 3;
			color.red = math::noise_range(v, num, 0, 255);
			color.green = math::noise_range(v + 1, num, 0, 255);
			color.blue = math::noise_range(v + 2, num, 0, 255);

			pos = pos - camera;
			renderer.draw_color(pos, size, color);
			renderer.draw_circle(pos, size.x * .45, color.invert());
		}

		for (auto i = 0; i < len; i += 2)
		{
			auto col = i % num;
			auto row = i / num;
			glm::vec2 pos = { col * size.x, row * size.y };

			pos = pos - camera;
			renderer.draw_texture(pos, size, texture, 1);
		}

		renderer.flush();

		show_debug_info();
	}

	void Test::show_debug_info()
	{
		renderer.flush();
		auto stats = gfx::Context::gpu_stats;

		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto time = ctx->main_clock.milliseconds();
		i32 ms = time;
		i32 seconds = ms / 1000;
		i32 minutes = seconds / 60;
		i32 hours = minutes / 60;

		minutes %= 60;
		seconds %= 60;
		ms %= 1000;

		auto pen = glm::vec2(8, 64 + 32 + 16);
		auto _style = gfx::FontStyle(15);
		_style.border_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_size = 0.125;
		renderer.draw_color(glm::vec2(104, 60), glm::vec2(200, 108), gfx::Color32::from_rgba(0, 1, 0));
		pen = renderer.write_format(pen, font, _style, "Time: %d:%d:%d:%d\n",
			hours, minutes, seconds, ms / 100
		);
		pen = renderer.write_format(pen, font, _style, "Delta: %f\n", ctx->main_clock.delta());
		pen = renderer.write_format(pen, font, _style,
			"Draw calls: %u\nTriangles: %u\nTextureSwitch: %u",
			drawcalls, triangles, switches
		);

		renderer.flush();
	}
}