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

		// camera = game::Camera2D(ctx.main_window.width, ctx.main_window.height);
		// camera.zoom(64);
		camera = game::Camera2D(160, 90);
		// camera = game::Camera2D(1, 1);

		root.transform.position = { 0, 0 };
		child_0.transform.position = { 2, 0 };
		child_1.transform.position = { -1, 0 };
		child_1_0.transform.position = { -1, 0 };

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

		float CAM_SPEED = 1;
		if (ip.is_key_pressed(KeyCode::R)) // reset camera
		{
			camera.transform = {};
		}

		glm::vec2 direction(0);
		if (ip.is_key_down(KeyCode::W)) // up
			direction.y = 1;
		if (ip.is_key_down(KeyCode::S))
			direction.y = -1;
		if (ip.is_key_down(KeyCode::A))
			direction.x = -1;
		if (ip.is_key_down(KeyCode::D))
			direction.x = 1;
		
		if (ip.is_key_down(KeyCode::Q))
			camera.transform.rotation -= delta;
		if (ip.is_key_down(KeyCode::E))
			camera.transform.rotation += delta;
		
		float ZOOM_SPEED = 10;
		if (ip.is_key_down(KeyCode::Z)) // zoom in
			camera.zoom(ZOOM_SPEED * delta);
		if (ip.is_key_down(KeyCode::X)) // zoom out
			camera.zoom(-ZOOM_SPEED * delta);
		
		if (ip.is_key_released(KeyCode::D0))
			camera.transform.scale = { 0, 0 };
		
		camera.move(CAM_SPEED * delta, direction / camera.transform.scale);

		if (ip.is_key_down(KeyCode::NP_PLUS))
			child_1.transform.rotation += delta;
		if (ip.is_key_down(KeyCode::NP_MINUS))
			child_1.transform.rotation -= delta;
		
		root.transform.rotation += delta * .5;
	}

	void Test::render()
	{
		auto view = camera.view_transform({1600, 900});

		auto num = 64;
		auto len = num * num;
		math::Transform2D tf;
		glm::vec2 size(.5);
		math::Random rgen(137);
		for (auto i = 0; i < len; i++)
		{
			auto row = i / num;
			auto col = i % num;

			tf.position = { col * size.x * 1.1, row * size.y * 1.1 };
			auto vtf = tf.apply_transform(view);

			gfx::Color8 color(rgen.range(0, 255));
			renderer.draw_color(vtf.position, size * vtf.scale, color,
				vtf.rotation
			);
		}

		game::GObject* scene[] = { &root, &child_0, &child_1, &child_1_0 };
		gfx::Color8 colors[] = { gfx::WHITE,
			gfx::RED, gfx::GREEN, gfx::BLUE
		};
		glm::vec2 pen = { 0, ctx->main_window.height };
		auto style = gfx::FontStyle(24);
		int length = sizeof(scene) / sizeof(scene[0]);
		for (auto i = 0; i < length; i++)
		{
			auto obj = scene[i];
			auto wtf = obj->world_transform();
			auto vtf = wtf.apply_transform(view);
			renderer.draw_color(vtf.position, size * vtf.scale,
				colors[i], vtf.rotation
			);
		}

		style.border_size = 0;
		pen = renderer.write_format(pen,
			font, style,
			"angle: %f\n", child_1.transform.rotation
		);
		auto _s = style;
		_s.border_size = .1;
		_s.border_color = gfx::RED;
		pen = renderer.write_format(pen, font, _s,
			"cam: [%.2f, %.2f] {%.2f, %.2f} <%.2f>\n",
			camera.transform.position.x, camera.transform.position.y,
			camera.transform.scale.x, camera.transform.scale.y,
			camera.transform.rotation
		);
		pen = renderer.write_format(pen, font, _s,
			"view: [%.2f, %.2f] {%.2f, %.2f} <%.2f>\n",
			view.position.x, view.position.y,
			view.scale.x, view.scale.y,
			view.rotation
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
		// _style.border_color = gfx::BLACK;
		_style.border_size = 0.125 * 0;
		renderer.draw_color(glm::vec2(104, 60), glm::vec2(200, 108),
			gfx::BLACK.with_alpha(.7)
		);
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