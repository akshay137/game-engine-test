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

		renderer.create(1024 * 64);
		renderer.set_clip_size(ctx.main_window.width, ctx.main_window.height);
		
		texture = res::load_texture("assets/logo.png");

		font = res::load_font("assets/firacode.atlas");

		camera = game::Camera2D(ctx.main_window.width, ctx.main_window.height);

		root.transform.position = { 0, 0 };
		child_0.transform.position = { 64, 0 };
		child_1.transform.position = { -64, 0 };
		child_1_0.transform.position = { -32, 0 };

		child_0.set_parent(root);
		child_1.set_parent(root);
		child_1_0.set_parent(child_1);
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
			camera.transform = {};

		glm::vec2 direction(0);
		if (ip.is_key_down(KeyCode::W)) // up
			direction.y = 1;
		if (ip.is_key_down(KeyCode::S))
			direction.y = -1;
		if (ip.is_key_down(KeyCode::A))
			direction.x = -1;
		if (ip.is_key_down(KeyCode::D))
			direction.x = 1;
		
		float ZOOM_SPEED = 10;
		if (ip.is_key_down(KeyCode::Z)) // zoom in
			camera.zoom(ZOOM_SPEED * delta);
		if (ip.is_key_down(KeyCode::X)) // zoom out
			camera.zoom(-ZOOM_SPEED * delta);
		
		camera.move(CAM_SPEED * delta, direction);

		if (ip.is_key_down(KeyCode::NP_PLUS))
			child_1.transform.rotation += delta;
		if (ip.is_key_down(KeyCode::NP_MINUS))
			child_1.transform.rotation -= delta;
		
		// root.transform.rotation += delta * .5;
	}

	void Test::render()
	{
		auto view = camera.view_transform();

		glm::vec2 size = { 16, 16 };
		game::GObject* scene[] = { &root, &child_0, &child_1, &child_1_0 };
		gfx::Color32 colors[] = { gfx::WHITE,
			gfx::RED, gfx::GREEN, gfx::BLUE
		};
		int length = sizeof(scene) / sizeof(scene[0]);
		for (auto i = 0; i < length; i++)
		{
			auto obj = scene[i];
			auto tf = obj->world_transform();
			tf = tf.apply_transform(view);
			renderer.draw_color(tf.position, size * tf.scale,
				colors[i],
				tf.rotation, glm::vec2(0)
			);
		}

		auto style = gfx::FontStyle(24);
		style.border_size = 0;
		auto pen = renderer.write_format({ 0, ctx->main_window.height },
			font, style,
			"angle: %f\n", child_1.transform.rotation
		);
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