#include "game.hpp"
#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/res/texture.hpp"

namespace game
{
	using namespace uhero;

	uhero::Result Game::load(uhero::Context&)
	{
		spritesheet = uhero::res::load_texture("assets/spritesheet.png");
		spritesheet.generate_mipmaps();
		spritesheet.set_filter(gfx::TextureFilter::MipmapLinear);

		auto res = uber.create(2048);
		if (Result::Success != res)
			return res;

		return Result::Success;
	}

	void Game::clear()
	{
		uber.clear();
		spritesheet.clear();
	}

	void Game::update(float)
	{
		auto& ip = ctx.input;
		if (ip.is_key_released(KeyCode::Escape))
			ctx.request_exit();
	}

	void Game::render()
	{
		glm::vec2 pos;
		pos.x = ctx.input.mouse.x;
		pos.y = ctx.main_window.height - ctx.input.mouse.y;
		float scale = 1.5;

		gfx::Sprite sprite(spritesheet);
		uber.draw_sprite(glm::vec2(0), sprite, scale);
		uber.draw_color(pos, glm::vec2(32), gfx::Color32::from_rgba(1, 0, 0));
		auto pen = uber.write(glm::vec2(0, ctx.main_window.height),
			ctx.get_system_font(), ctx.style_normal,
			"float aspect = logo.get_aspect_ratio();\n"
		);
		pen = uber.write(pen,
			ctx.get_system_font(), ctx.style_normal,
			"Hello World.\nfrom uber shader.\n"
		);
		pen = uber.write(pen, ctx.get_system_font(), ctx.style_normal,
			"spritesheet: [%dx%d] | scale: %f\n",
			spritesheet.width, spritesheet.height, scale
		);
		pen = uber.write(pen, ctx.get_system_font(), ctx.style_normal,
			"mouse: <%.1fx%.1f>", pos.x, pos.y
		);

		uber.flush();
	}
}