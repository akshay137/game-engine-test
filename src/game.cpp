#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"

namespace game
{
	using namespace uhero;

	uhero::Result Game::load(uhero::Context&)
	{
		spritesheet = res::load_texture("assets/spritesheet.png");
		spritesheet.set_filter(gfx::TextureFilter::Nearest);

		auto res = uber.create(16);
		if (Result::Success != res)
			return res;

		font = res::load_font("assets/cascadia.atlas");
		style = gfx::FontStyle(16);

		return Result::Success;
	}

	void Game::clear()
	{
		uber.clear();
		spritesheet.clear();
		font.clear();
	}

	void Game::update(float)
	{
		auto& ip = ctx.input;
		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();
	}

	void Game::render()
	{
		const i32 w = ctx.main_window.width;
		const i32 h = ctx.main_window.height;

		glm::vec2 pos;
		pos.x = ctx.input.mouse.x;
		pos.y = ctx.input.mouse.y;

		uber.draw_texture(glm::vec2(0), spritesheet);
		
		auto pen = uber.write(glm::vec2(0, ctx.main_window.height),
			font, style,
			"float aspect = logo.get_aspect_ratio();\n"
		);
		pen = uber.write(pen, font, style,
			"Hello World!\nfrom uber shader.\n"
		);
		pen = uber.write(pen, font, style,
			"spritesheet: [%dx%d] | hmm...\n",
			spritesheet.width, spritesheet.height
		);
		pen = uber.write(pen, font, style,
			"mouse: <%.1fx%.1f>", pos.x, pos.y
		);

		uber.flush();
	}
}