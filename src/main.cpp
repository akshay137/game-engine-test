#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/pso.hpp"

#include <glad/glad.h>

using namespace uhero;
struct Game : uhero::Level
{
	// data_begin
	// Texture logo;
	// vec2 position;
	// data_end
	gfx::PSO quad;

	uhero::Result load(uhero::Context& ctx) override
	{
		gfx::VertexLayout layout {};
		layout.add_attribute(gfx::VertexAttribute::Vec2);
		layout.add_attribute(gfx::VertexAttribute::Vec2);
		layout.add_attribute(gfx::VertexAttribute::Vec3);

		auto res = quad.create(layout, "assets/quad.vert", "assets/quad.frag");

		UH_INFO("Loaded Game\n");
		return uhero::Result::Success;
	}

	void clear(uhero::Context& ctx) override
	{
		quad.clear();
		UH_INFO("Cleared Game\n");
	}

	void update(uhero::Context& ctx, float delta) override
	{
		UH_INFO("delta: %.4f\r", delta);
	}

	void render(uhero::Context& ctx) override
	{
		auto& gfx = ctx.gfx;
		quad.make_current();

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};

int main(int argc, char** args)
{
	uhero::Context engine = uhero::Context::create_context(argc, args);

	Game game {};
	engine.set_current_level(&game);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}