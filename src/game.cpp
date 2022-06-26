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
		auto res = from + (to - from) * time;
		return res;
	}
	
	template <typename T>
	T clamp(const T& v, const T& lower, const T& upper)
	{
		return std::min(upper, std::max(v, lower));
	}

	glm::vec2 snap_to_pixel(glm::vec2 pos)
	{
		auto res = glm::vec2(round(pos.x), round(pos.y));
		return res;
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

		res = map.load_from_file("assets/test.map");
		if (Result::Success != res)
		{
			return res;
		}

		camera = glm::vec4(0, 0, GAME_SIZE.x, GAME_SIZE.y);

		player.position = glm::vec2 (128, 64);
		player.velocity = glm::vec2(0);
		player.sprite_id = glm::ivec2(0, 0);

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		map.clear();
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
		
		constexpr float MOVE_SPEED = 8.0f;
		glm::vec2 direction(0);
		if (ip.is_key_down(KeyCode::A)) // left
			direction.x = -1;
		if (ip.is_key_down(KeyCode::D)) // right
			direction.x = 1;

		player.velocity += direction * MOVE_SPEED * delta;
		auto limit = glm::vec2(MOVE_SPEED * .5);
		player.velocity = clamp(player.velocity, -limit, limit);
		player.position += player.velocity;

		if (0 == direction.x)
			player.velocity = lerp(player.velocity, glm::vec2(0), damp * delta);

		camera.x = lerp(camera.x, player.position.x, 7 * delta);
		camera.y = lerp(camera.y, player.position.y, delta);
		glm::vec2 half_size = { camera.z / 2, camera.w / 2 };
		float ratio = map.tileset.tile_size;
		camera.x = clamp(camera.x, half_size.x, ratio * map.width - half_size.x);
		camera.y = clamp(camera.y, half_size.y, ratio * map.height - half_size.y);
	}

	void Game::render()
	{
		// draw game
		ctx.gfx.use_framebuffer(game_fbo);
		float sky_color[] = { .3, .7, .9, 1 };
		game_fbo.clear_buffers(sky_color, 1, 0);
		
		draw_tile_map(map, camera);
		glm::vec2 ppos = player.position - get_camera_offset();
		uber.draw_texture(snap_to_pixel(ppos), glm::vec2(CHAR_SIZE, CHAR_SIZE),
			tx_characters,
			get_spritesheet_source(player.sprite_id.x, player.sprite_id.y, CHAR_SIZE)
		);

		uber.flush();

		// present game
		ctx.gfx.use_default_framebuffer(ctx.main_window);
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);
		glm::vec2 pos = screen * .5f;
		glm::vec2 size(game_fbo.width, game_fbo.height);
		float game_aspect = GAME_SIZE.x / GAME_SIZE.y;
		float height_ratio = screen.y / GAME_SIZE.y;
		size.y = GAME_SIZE.y * height_ratio;
		size.x = size.y * game_aspect;

		if (size.x > screen.x)
		{
			float width_ratio = screen.x / size.x;
			size.x *= width_ratio;
			size.y = size.x * (GAME_SIZE.y / GAME_SIZE.x);
		}
		uber.draw_texture(pos, size, game_fbo.color[0],
			glm::vec4(0, game_fbo.height, game_fbo.width, -game_fbo.height)
		);
		uber.flush();

		// game debug info
		if (debug_info_enabled)
			show_debug_info();
		
		auto pen = screen_to_world(glm::vec2(0), screen);
		pen = uber.write_format(pen, font, style, "cam: [%dx%d]{%dx%d}\n",
			(int)camera.x, (int)camera.y,
			(int)camera.z, (int)camera.w
		);
		pen = uber.write_format(pen, font, style,
			"player: pos[%.2f, %.2f] | v[%.2f, %.2f]\n",
			player.position.x, player.position.y,
			player.velocity.x, player.velocity.y
		);

		uber.flush();
	}

	void Game::draw_tile_map(const TileMap& tmap, glm::vec4 camera)
	{
		const auto& tset = tmap.tileset;
		glm::vec2 size = { tset.tile_size, tset.tile_size };
		glm::vec4 src = { 0, 0, size.x, size.y };
		glm::vec2 cam_offset = get_camera_offset();
		for (auto i = 0; i < tmap.tile_count; i++)
		{
			const auto& tile = tmap.tiles[i];
			int sx, sy;
			tset.get_texture_offset(tile.id, sx, sy);
			src.x = sx;
			src.y = sy;

			glm::vec2 pos = { tile.x, tmap.height - tile.y };
			auto view_pos = pos * size - cam_offset;
			view_pos.x += (size.x / 2);
			view_pos.y -= (size.y / 2);
			if (view_pos.x + size.x * .5 < 0) continue;
			if (view_pos.x > GAME_SIZE.x + size.x * .5) continue;
			if (view_pos.y + size.y * .5 < 0) continue;
			if (view_pos.y > GAME_SIZE.y + size.y * .5) continue;

			// snap to pixel grid, removes some sprite artifacts
			view_pos.x = round(view_pos.x);
			view_pos.y = round(view_pos.y);
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