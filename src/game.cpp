#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"

#include <string_view>

namespace game
{
	using namespace uhero;

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

		return Result::Success;
	}

	void Game::clear()
	{
		uber.clear();
		spritesheet.clear();
		font.clear();
		firacode.clear();
	}

	void Game::update(float)
	{
		auto& ip = ctx.input;
		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();	
	}

	void Game::render()
	{
		float card_size = 256 + 64;
		glm::vec2 pos = glm::vec2(1);

		uber.draw_color(screen_to_world(glm::vec2(1200, 0)), glm::vec2(64, -64),
			gfx::Color32::from_rgba(1, 1, 0)
		);
		auto size = show_image_card(pos, card_size, spritesheet, "spritesheet");

		uber.flush();

		// debug info
		auto stats = gfx::Context::gpu_stats;
		auto drawcalls = stats.draw_calls;
		auto triangles = stats.triangle_count;
		auto switches = stats.texture_switch;

		auto pen = glm::vec2(8, 64 + 32);
		auto _style = style;
		_style.size = 15.0 * gfx::PT_TO_PIXEL;
		_style.border_size = 0.005;
		uber.draw_color(glm::vec2(4), glm::vec2(256, pen.y), gfx::Color32::from_rgba(0, 1, 0));
		pen = uber.write_format(pen, font, _style, "delta: %f\n", ctx.main_clock.delta());
		pen = uber.write_format(pen, font, _style,
			"Draw calls: %u\nTriangles: %u\nTextureSwitch: %u",
			drawcalls, triangles, switches
		);

		uber.flush();
	}

	glm::vec2 Game::screen_to_world(glm::vec2 pos)
	{
		glm::vec2 res(pos.x, ctx.main_window.height - pos.y);
		return res;
	}

	glm::vec2 Game::show_image_card(glm::vec2 pos, float size,
		const uhero::gfx::Texture& texture,
		std::string_view title
	)
	{
		const auto& _font = firacode;
		float scale = size / texture.width;

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
		uber.draw_color(screen_to_world(cpos), quad, gfx::Color32::from_rgba(0, 1, 1));

		uber.draw_color(screen_to_world(glm::vec2(pos.x + 3, pos.y + text_height)), box,
			gfx::Color32::from_rgba(1, 1, 1)
		);
		
		glm::vec2 tpos(pos.x + 3, pos.y);
		auto pen = uber.write(screen_to_world(tpos), _font, _style, title.size(), title.data());

		pen.x = pos.x + 3;
		pen.y = pos.y + text_height;
		pen.y += texture.height * scale + 2;
		uber.draw_texture(screen_to_world(pen), texture, scale);

		return quad;
	}
}