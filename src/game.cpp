#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/font_atlas.hpp"

#include <cmath>

namespace game
{
	using namespace uhero;

	glm::vec2 snap_to_pixel(glm::vec2 pos)
	{
		auto res = glm::vec2(round(pos.x), round(pos.y));
		return res;
	}

	b2Vec2 snap_to_pixel(b2Vec2 pos)
	{
		auto res = b2Vec2(round(pos.x), round(pos.y));
		return res;
	}

	b2Body* create_static_body(b2World* world, float x, float y, float w, float h)
	{
		b2BodyDef def;
		def.type = b2_staticBody;
		def.position.Set(x, y);

		auto* body = world->CreateBody(&def);

		b2PolygonShape shape;
		shape.SetAsBox(w * .5, h * .5);
		body->CreateFixture(&shape, 0);
		return body;
	}

	glm::vec4 physics_to_view(b2Body* body, float scale)
	{
		glm::vec4 view(0);
		auto pos = body->GetPosition();
		view.x = pos.x * scale;
		view.y = pos.y * scale;

		auto* fixture = body->GetFixtureList();
		if (nullptr == fixture) return view;

		auto aabb = fixture[0].GetAABB(0).GetExtents();
		view.z = aabb.x * scale * 2;
		view.w = aabb.y * scale * 2;
		return view;
	}

	b2Body* create_dynamic_body(b2World* world, float x, float y, float w, float h)
	{
		b2BodyDef def;
		def.type = b2_dynamicBody;
		def.position.Set(x, y);

		auto* body = world->CreateBody(&def);

		b2PolygonShape shape;
		shape.SetAsBox(w * .5, h * .5);
		body->CreateFixture(&shape, 0);
		return body;
	}

	uhero::Result Game::load(uhero::Context&)
	{
		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(16);
		style.border_size = 0.1;
		style.border_color = gfx::Color32::from_rgba(1, 0, 0);

		b2Vec2 gravity(0, -9.8);
		world = new b2World(gravity);
		DUMPF(s2v_ratio);

		ground = create_static_body(world, 0, -5, 10, 1);
		ball = create_dynamic_body(world, 0, 0, 1, 1);

		// 0 -> logic controlled
		// 1 -> static body
		ball->SetUserData((void*)(0));
		ground->SetUserData((void*)(1));

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		delete world;
		uber.clear();
		font.clear();
	}

	void Game::update(float delta)
	{
		auto& ip = ctx.input;
		auto& audio = ctx.audio;

		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();
		if (ip.is_key_released(KeyCode::Tilde))
			debug_info_enabled = !debug_info_enabled;
		
		if (ip.is_key_pressed(KeyCode::Space))
		{
			ball->ApplyForce(b2Vec2(0, 100), b2Vec2(0, 0), true);
		}
		world->Step(delta, 8, 3);
	}

	void Game::render()
	{
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);
		glm::vec2 co(screen.x * .5, screen.y * .5);

		// draw game
		auto bpos = physics_to_view(ball, s2v_ratio);
		auto gpos = physics_to_view(ground, s2v_ratio);

		auto bcolor = gfx::Color32::from_rgba(1, 0, 0);
		auto gcolor = gfx::Color32::from_rgba(1, 1, 1);

		uber.draw_color(co, { 25, 25 }, gfx::Color32(255));
		uber.draw_color({ bpos.x + co.x, bpos.y + co.y }, { bpos.z, bpos.w }, bcolor);
		uber.draw_color({ gpos.x + co.x, gpos.y + co.y }, { gpos.z, gpos.w }, gcolor);

		uber.flush();

		// game debug info
		if (debug_info_enabled)
			show_debug_info();
		
		auto pen = screen_to_world(glm::vec2(0), screen);
		pen = uber.write_format(pen, font, style, "sim2view: %f\n", s2v_ratio);
		pen = uber.write_format(pen, font, style, "pos [%.2f, %.2f | %.2f, %.2f]\n",
			bpos.x, bpos.y, bpos.z, bpos.w
		);
		pen = uber.write_format(pen, font, style, "ground [%.2f, %.2f | %.2f, %.2f]\n",
			gpos.x, gpos.y, gpos.z, gpos.w
		);
		uber.flush();
	}

	void Game::show_debug_info()
	{
		auto stats = gfx::Context::gpu_stats;

		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto time = ctx.main_clock.nanoseconds();
		i32 ms = time / 1000000;
		i32 seconds = ms / 1000;
		i32 minutes = seconds / 60;
		i32 hours = minutes / 60;

		minutes %= 60;
		seconds %= 60;
		ms %= 1000;

		auto pen = glm::vec2(8, 64 + 32 + 16);
		auto _style = style;
		_style.size = 15.0 * gfx::PT_TO_PIXEL;
		_style.border_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_size = 0.1;
		uber.draw_color(glm::vec2(104, 60), glm::vec2(200, 108), gfx::Color32::from_rgba(0, 1, 0));
		pen = uber.write_format(pen, font, _style, "Time: %d:%d:%d:%d\n",
			hours, minutes, seconds, ms / 100
		);
		pen = uber.write_format(pen, font, _style, "Delta: %f\n", ctx.main_clock.delta());
		pen = uber.write_format(pen, font, _style,
			"Draw calls: %u\nTriangles: %u\nTextureSwitch: %u",
			drawcalls, triangles, switches
		);

		uber.flush();
	}
}