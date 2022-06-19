#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/memory/memory.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"
#include "uhero/sfx/sfx.hpp"

#include <string_view>
#include <SDL2/SDL_audio.h>

namespace game
{
	using namespace uhero;

	gfx::FrameBuffer fbo;
	sfx::AudioBuffer hit;
	sfx::AudioBuffer item;
	i32 hit_channel = 0;
	i32 item_channel = 0;

	sfx::AudioBuffer load_audio(const char* file)
	{
		SDL_AudioSpec spec;
		u8* wbuff;
		u32 size;
		auto* res = SDL_LoadWAV(file, &spec, &wbuff, &size);
		if (!res)
		{
			UHSDL_ERROR(SDL_LoadWAV);
			assert(false);
		}

		UH_INFO("Format: %x\n", spec.format);
		UH_INFO("BitSize: %d | Float: %s | Endian: %s | Signed: %s\n",
			SDL_AUDIO_BITSIZE(spec.format),
			SDL_AUDIO_ISFLOAT(spec.format) ? "yes" : "no",
			SDL_AUDIO_ISBIGENDIAN(spec.format) ? "big" : "little",
			SDL_AUDIO_ISSIGNED(spec.format) ? "true" : "false"
		);
		UH_INFO("Frequency: %d\n", spec.freq);
		UH_INFO("SampleRate: %d\n", spec.samples);
		UH_INFO("Channels: %d\n", spec.channels);

		i32 sample_length = size / (sizeof(i16) * spec.channels);
		UH_INFO("samples: %d | size: %d\n", sample_length, size);

		sfx::AudioBuffer buffer {};
		buffer.buffer = UH_ALLOCATE_TYPE(sfx::sample_type, sample_length);
		buffer.size = sample_length;
		buffer.offset = 0;
		buffer.loop = false;

		constexpr float MAX_SIGNED = 0x7fff;
		for (i32 i = 0; i < buffer.size; i++)
		{
			const i16* audio_buffer = reinterpret_cast<i16*>(wbuff);
			float sample = 0;
			i16 L = audio_buffer[i * spec.channels + 0];

			sample = L / MAX_SIGNED;

			buffer.buffer[i] = sample;
		}

		SDL_FreeWAV(wbuff);
		return buffer;
	}

	uhero::Result Game::load(uhero::Context&)
	{
		spritesheet = res::load_texture("assets/spritesheet.png");
		spritesheet.set_filter(gfx::TextureFilter::Nearest);

		auto res = uber.create(1024);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/cascadia.atlas");
		firacode = res::load_font("assets/firacode.atlas");
		style = gfx::FontStyle(16);
		style.border_size = 0;

		gfx::FBDescriptor desc {};
		desc.add_color_attachment(gfx::PixelFormat::RGBA_F32);
		desc.set_depth_attachment(gfx::PixelFormat::Depth24Stencil8);
		res = fbo.create(desc, 640, 360);
		if (Result::Success != res)
		{
			return res;
		}

		hit = load_audio("assets/sample1.wav");
		item = load_audio("assets/sample.wav");

		return Result::Success;
	}

	void Game::clear()
	{
		ctx.audio.pause();
		
		uber.clear();
		spritesheet.clear();
		font.clear();
		firacode.clear();
		fbo.clear();
		UH_FREE(hit.buffer);
		UH_FREE(item.buffer);
	}

	void Game::update(float)
	{
		auto& ip = ctx.input;
		auto& audio = ctx.audio;

		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();
		
		if (ip.is_key_released(KeyCode::A)) // attack
		{
			UH_INFO("Attacking\n");
			hit_channel = audio.play_buffer(hit, .5);
		}
		if (ip.is_key_released(KeyCode::S)) // item
		{
			item_channel = audio.play_buffer(item, 0.25);
		}
	}

	void Game::render()
	{
		ctx.gfx.use_framebuffer(fbo);

		i32 SIZE = 16;
		glm::vec2 bsize(SIZE);

		glm::vec2 action_center = glm::vec2(fbo.width / 2, fbo.height / 2);
		glm::vec2 BUTTONS[4] = {
			glm::vec2(0, 1), // A
			glm::vec2(1, 0), // B
			glm::vec2(-1, 0), // X
			glm::vec2(0, -1), // Y
		};

		glm::vec2 screen(fbo.width, fbo.height);

		action_center.x -= bsize.x * 3.0f;
		for (auto i = 0; i < 4; i++)
		{
			auto pos = screen_to_world(action_center + BUTTONS[i] * bsize.x, screen);
			uber.draw_texture(pos, bsize, spritesheet,
				get_spritesheet_source(i, 0, SIZE)
			);
		}

		action_center.x += bsize.x * 3.0f;
		for (auto i = 0; i < 4; i++)
		{
			auto pos = screen_to_world(action_center + BUTTONS[i] * bsize.x, screen);
			uber.draw_texture(pos, bsize, spritesheet,
				get_spritesheet_source(4 + i, 0, SIZE)
			);
		}

		uber.flush();

		// final render
		ctx.gfx.use_default_framebuffer(ctx.main_window);
		uber.draw_texture(glm::vec2(0),
			glm::vec2(ctx.main_window.width, ctx.main_window.height),
			fbo.color[0], glm::vec4(0, fbo.height, fbo.width, -fbo.height)
		);
		uber.flush();

		show_debug_info();
	}

	glm::vec2 Game::screen_to_world(glm::vec2 pos, glm::vec2 screen)
	{
		glm::vec2 res(pos.x, screen.y - pos.y);
		return res;
	}

	glm::vec2 Game::show_image_card(glm::vec2 pos, float size,
		const uhero::gfx::Texture& texture,
		std::string_view title, bool y_flipped
	)
	{
		const auto& _font = firacode;
		float scale = size / texture.width;
		glm::vec2 screen(ctx.main_window.width, ctx.main_window.height);

		glm::vec2 s(0);
		auto _style = gfx::FontStyle(16);
		_font.get_box_size(title.size(), title.data(), _style, s.x, s.y);
		if (s.x > size)
		{
			float new_size = (_style.size / gfx::PT_TO_PIXEL) * (size / s.x);
			_style = gfx::FontStyle(new_size);
		}

		_style.text_color = gfx::Color32::from_rgba(0, 0, 0);
		_style.border_color = gfx::Color32::from_rgba(1, 0, 0);
		_style.border_size = 0.01;

		float sscale = _font.size_scale(_style.size);
		float text_height = (_font.line_height + _style.line_spacing * 3) * sscale;
		glm::vec2 box(size, text_height * 0.95);
		glm::vec2 quad = glm::vec2(size + 6,
			texture.height * scale + 4 + text_height
		);

		glm::vec2 cpos(pos.x, pos.y + quad.y);
		uber.draw_color(screen_to_world(cpos, screen), quad, gfx::Color32::from_rgba(0, 1, 1));

		uber.draw_color(screen_to_world(glm::vec2(pos.x + 3, pos.y + text_height), screen), box,
			gfx::Color32::from_rgba(1, 1, 1)
		);
		
		glm::vec2 tpos(pos.x + 3, pos.y);
		auto pen = uber.write(screen_to_world(tpos, screen), _font, _style, title.size(), title.data());

		pen.x = pos.x + 3;
		pen.y = pos.y + text_height;
		pen.y += texture.height * scale + 2;
		glm::vec4 flipped_src(0, texture.height, texture.width, -texture.height);
		glm::vec2 tex_size(texture.width * scale, texture.height * scale);
		if (y_flipped)
			uber.draw_texture(screen_to_world(pen, screen), tex_size, texture, flipped_src);
		else
			uber.draw_texture(screen_to_world(pen, screen), texture, scale);

		return quad;
	}

	void Game::show_debug_info()
	{
		auto stats = gfx::Context::gpu_stats;

		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto pen = glm::vec2(8, 64 + 32 + 24);
		auto _style = style;
		_style.size = 15.0 * gfx::PT_TO_PIXEL;
		_style.border_size = 0.005;
		uber.draw_color(glm::vec2(4), glm::vec2(256, pen.y), gfx::Color32::from_rgba(0, 1, 0));
		pen = uber.write_format(pen, font, _style, "time: %.3f\n", ctx.main_clock.seconds());
		pen = uber.write_format(pen, font, _style, "delta: %f\n", ctx.main_clock.delta());
		pen = uber.write_format(pen, font, _style,
			"Draw calls: %u\nTriangles: %u\nTextureSwitch: %u",
			drawcalls, triangles, switches
		);

		uber.flush();
	}
}