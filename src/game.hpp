#ifndef GAME_H__
#define GAME_H__ 1

#include "uhero/level.hpp"
#include "uhero/gfx/renderer.hpp"

namespace game
{
	struct Game : public uhero::Level
	{
		uhero::Context& ctx;
		uhero::gfx::Renderer uber;

		uhero::gfx::Texture spritesheet;

		Game(uhero::Context& ctx)
			: ctx{ctx}
		{}

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

	};
}

#endif