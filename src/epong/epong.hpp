#ifndef EPONG_H__
#define EPONG_H__ 1

#include "../uhero/application.hpp"
#include "../uhero/gfx/renderer.hpp"

namespace epong
{
	struct Game : public uhero::IApplication
	{
		uhero::Context* ctx;
		uhero::gfx::Renderer uber;
		uhero::gfx::Font font;

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;
	};
}

#endif