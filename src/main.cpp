#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/pso.hpp"
#include "uhero/gfx/buffer.hpp"
#include "uhero/res/texture.hpp"

#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex2d
{
	glm::vec2 position;
	glm::vec2 uv;
	glm::vec3 color;

	Vertex2d(glm::vec2 position)
		: position{position}
	{}

	Vertex2d(glm::vec2 position, glm::vec2 uv)
		: position{position}, uv{uv}
	{}

	Vertex2d(glm::vec2 position, glm::vec3 color)
		: position{position}, color{color}
	{}

	Vertex2d(glm::vec2 position, glm::vec2 uv, glm::vec3 color)
		: position{position}, uv{uv}, color{color}
	{}
};

struct RenderState
{
	glm::mat4 orthographic;
	glm::vec4 viewport;
};

using namespace uhero;
struct Game : uhero::Level
{
	gfx::PSO quad;
	gfx::TBuffer<Vertex2d> vbuffer;
	gfx::TBuffer<u16> ibuffer;
	gfx::TBuffer<RenderState> rsbuffer;
	RenderState rstate;
	gfx::Texture logo;

	uhero::Result load(uhero::Context& ctx) override
	{
		gfx::VertexLayout layout {};
		layout.add_attribute(gfx::VertexAttribute::Vec2);
		layout.add_attribute(gfx::VertexAttribute::Vec2);
		layout.add_attribute(gfx::VertexAttribute::Vec3);

		auto res = quad.create(layout, "assets/quad.vert", "assets/quad.frag");

		Vertex2d vertices[] = {
			Vertex2d(glm::vec2(-0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
			Vertex2d(glm::vec2(0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
			Vertex2d(glm::vec2(0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(0, 0, 1)),
			Vertex2d(glm::vec2(-0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(1, 0, 1))
		};
		vbuffer.create(gfx::BufferType::Static, 4, vertices);

		u16 indices[] = {
			0, 1, 2,
			2, 0, 3
		};
		ibuffer.create(gfx::BufferType::Static, 6, indices);

		float w = ctx.main_window.width;
		float h = ctx.main_window.height;
		rstate.orthographic = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);
		rstate.viewport = glm::vec4(0.0f, 0.0f, w, h);
		rsbuffer.create(gfx::BufferType::Dynaminc, 1, &rstate);

		logo = res::load_texture("assets/logo.png");

		UH_INFO("Loaded Game\n");
		return uhero::Result::Success;
	}

	void clear(uhero::Context& ctx) override
	{
		logo.clear();
		ibuffer.clear();
		vbuffer.clear();
		rsbuffer.clear();
		quad.clear();
		UH_INFO("Cleared Game\n");
	}

	void update(uhero::Context& ctx, float delta) override
	{
		UH_INFO("delta: %.4f\r", delta);

		float w = ctx.main_window.width;
		float h = ctx.main_window.height;
		rstate.orthographic = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);
		rstate.viewport = glm::vec4(0.0f, 0.0f, w, h);
		rsbuffer.update(&rstate);
	}

	void render(uhero::Context& ctx) override
	{
		auto& gfx = ctx.gfx;

		logo.bind_slot(0);
		rsbuffer.bind_base(gfx::BufferBase::Uniform, 0);
		quad.make_current();
		quad.set_vertex_buffer(vbuffer.get_buffer(), 0, 0, vbuffer.stride());
		quad.set_index_buffer(ibuffer.get_buffer());

		glDrawElements(GL_TRIANGLES, ibuffer.count, GL_UNSIGNED_SHORT, 0);
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