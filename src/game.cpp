#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/memory/memory.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"
#include "uhero/sfx/sfx.hpp"
#include "uhero/res/audio.hpp"

#include <cmath>

namespace game
{
	using namespace uhero;

	template <typename T>
	T lerp(const T& from, const T& to, float time)
	{
		T temp = from + (to - from) * time;
		return temp;
	}

	template <typename T>
	T clamp(const T& v, const T& lower, const T& upper)
	{
		return std::min(upper, std::max(v, lower));
	}

	float snap_to(const float v, const float inc)
	{
		auto div = v / inc;
		return round(div) * inc;
	}

	uhero::Result Game::load(uhero::Context&)
	{
		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		gfx::FBDescriptor descriptor;
		descriptor.add_color_attachment(gfx::PixelFormat::RGBA8);
		descriptor.set_depth_attachment(gfx::PixelFormat::Depth24Stencil8);
		res = game_fbo.create(descriptor, GAME_SIZE.x, GAME_SIZE.y);

		font = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(16);
		style.border_size = 0.1;
		style.border_color = gfx::Color32::from_rgba(1, 0, 0);

		tx_characters = res::load_texture("assets/characters.png");
		tx_characters.set_filter(gfx::TextureFilter::Linear);

		tx_tiles = res::load_texture("assets/tiles.png");
		tx_tiles.set_filter(gfx::TextureFilter::Linear);

		map.load_from_file("assets/test.map");

		map_camera = glm::vec4(0, 0,
			GAME_SIZE.x / map.tileset.tile_size,
			GAME_SIZE.y / map.tileset.tile_size
		);

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		map.clear();
		tx_tiles.clear();
		tx_characters.clear();
		game_fbo.clear();
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
		
		constexpr float CAM_SPEED = 32.0f;
		if (ip.is_key_down(KeyCode::A)) // left
			map_camera.x -= CAM_SPEED * delta;
		if (ip.is_key_down(KeyCode::D)) // right
			map_camera.x += CAM_SPEED * delta;
		
		if (ip.is_key_down(KeyCode::W)) // up
			map_camera.y += CAM_SPEED * delta;
		if (ip.is_key_down(KeyCode::S)) // down
			map_camera.y -= CAM_SPEED * delta;

		glm::vec2 half_size = { map_camera.z / 2, map_camera.w / 2 };
		map_camera.x = clamp(map_camera.x, half_size.x - 1, map.width - half_size.x + 1);
		map_camera.y = clamp(map_camera.y, half_size.y + 1, map.height - half_size.y + 1);
	}

	void Game::render()
	{
		// draw game
		ctx.gfx.use_framebuffer(game_fbo);
		float sky_color[] = { .3, .7, .9, 1 };
		game_fbo.clear_buffers(sky_color, 1, 0);
		draw_tile_map(map, map_camera);
		uber.flush();

		// present game
		ctx.gfx.use_default_framebuffer(ctx.main_window);
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);
		glm::vec2 fpos(0);
		glm::vec2 size(screen);
		float game_aspect = GAME_SIZE.y / GAME_SIZE.x;
		float game_to_screen_ratio = screen.y / GAME_SIZE.y;
		size.x = GAME_SIZE.x * game_to_screen_ratio;
		fpos.x += (screen.x - size.x) / 2;
		uber.draw_texture(fpos, size, game_fbo.color[0],
			glm::vec4(0, game_fbo.height, game_fbo.width, -game_fbo.height)
		);

		// game debug info
		auto pen = screen_to_world(glm::vec2(0), screen);
		pen = uber.write_format(pen, font, style, "cam: [%dx%d]{%dx%d}\n",
			(int)map_camera.x, (int)map_camera.y,
			(int)map_camera.z, (int)map_camera.w
		);

		uber.flush();
		if (debug_info_enabled)
			show_debug_info();
	}

	void Game::draw_tile_map(const TileMap& tmap, glm::vec4 camera)
	{
		const auto& tset = tmap.tileset;
		glm::vec2 size = { tset.tile_size, tset.tile_size };
		glm::vec4 src = { 0, 0, size.x, size.y };
		glm::vec2 cam_offset = { camera.x - camera.z / 2, camera.y - camera.w / 2 };
		for (auto i = 0; i < tmap.tile_count; i++)
		{
			const auto& tile = tmap.tiles[i];
			int sx, sy;
			tset.get_texture_offset(tile.id, sx, sy);
			src.x = sx;
			src.y = sy;

			glm::vec2 pos = { tile.x, tmap.height - tile.y };
			auto view_pos = (pos - cam_offset) * size;
			view_pos.x = snap_to(view_pos.x, map.tileset.tile_size);
			view_pos.y = snap_to(view_pos.y, map.tileset.tile_size);
			if (view_pos.x < 0) continue;
			if (view_pos.x > GAME_SIZE.x) continue;
			if (view_pos.y < 0) continue;
			if (view_pos.y > GAME_SIZE.y) continue;
			uber.draw_texture(view_pos, size, tset.atlas, src);

			// // debug view
			// auto rigid = gfx::Color32::from_rgba(1, 0, 0);
			// auto collect = gfx::Color32::from_rgba(0, 1, 0);
			// auto interact = gfx::Color32::from_rgba(0, 0, 1);
			// auto color = gfx::Color32::from_rgba(1, 1, 1);
			// if (tmap.tile_has_flag(tile.id, TileFlags::Static))
			// 	color = rigid;
			// else if (tmap.tile_has_flag(tile.id, TileFlags::Collect))
			// 	color = collect;
			// else if (tmap.tile_has_flag(tile.id, TileFlags::Interactable))
			// 	color = interact;
			// uber.draw_texture(view_pos, size, tset.atlas, src, 1, color);
		}
	}

	glm::vec2 Game::screen_to_world(glm::vec2 pos, glm::vec2 screen)
	{
		glm::vec2 res(pos.x, screen.y - pos.y);
		return res;
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

		auto pen = glm::vec2(8, 64 + 32 + 24);
		auto _style = style;
		_style.size = 15.0 * gfx::PT_TO_PIXEL;
		_style.border_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_size = 0.1;
		uber.draw_color(glm::vec2(4), glm::vec2(256, pen.y), gfx::Color32::from_rgba(0, 1, 0));
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