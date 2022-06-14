#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "uhero/gfx/pso.hpp"
#include "uhero/gfx/buffer.hpp"
#include "uhero/gfx/batch_renderer.hpp"
#include "uhero/gfx/text_renderer.hpp"
#include "uhero/file.hpp"
#include "uhero/res/texture.hpp"
#include "uhero/res/font_atlas.hpp"
#include "uhero/memory/memory.hpp"

#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct RenderState
{
	glm::mat4 orthographic;
	glm::vec4 viewport;
};

using namespace uhero;
struct Game : uhero::Level
{
	Context* ctx;
	gfx::TBuffer<RenderState> rsbuffer;
	RenderState rstate;
	gfx::Texture logo;
	gfx::BatchRenderer batch;
	gfx::TextRenderer text;
	glm::vec4 clip;
	i32 size = 512 + 256;

	char* code;
	const char* file = "premake5.lua";

	uhero::Result load(uhero::Context& ctx) override
	{
		float w = ctx.main_window.width;
		float h = ctx.main_window.height;
		rstate.orthographic = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);
		rstate.viewport = glm::vec4(0.0f, 0.0f, w, h);
		rsbuffer.create(gfx::BufferType::Dynaminc, 1, &rstate);

		logo = res::load_texture("assets/logo.png");
		logo.set_filter(gfx::TextureFilter::Nearest);

		float aspect = logo.get_aspect_ratio();
		clip = glm::vec4(0, 0, size, size * aspect);

		batch.create(1024);
		text.create(1024);

		auto buffer_size = Memory::kilobytes_to_bytes(1);
		code = UH_ALLOCATE_TYPE(char, buffer_size);
		auto read = File::read_full(file, code, buffer_size);
		code[read] = 0;

		this->ctx = &ctx;

		UH_DUMP_ALL_ALLOCATIONS();
		UH_INFO("Loaded Game\n");
		return uhero::Result::Success;
	}

	void clear() override
	{
		UH_FREE(code);
		batch.clear();
		text.clear();
		logo.clear();
		rsbuffer.clear();
		UH_INFO("Cleared Game\n");
	}

	void update(float delta) override
	{
		UH_INFO("delta: %.4f\r", delta);

		float w = ctx->main_window.width;
		float h = ctx->main_window.height;
		rstate.orthographic = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);
		rstate.viewport = glm::vec4(0.0f, 0.0f, w, h);
		rsbuffer.update(&rstate);
	}

	void render() override
	{
		logo.bind_slot(0);
		rsbuffer.bind_base(gfx::BufferBase::Uniform, 0);

		batch.begin(logo);
		batch.draw_sprite(glm::vec2(0), glm::vec4(0, 0, logo.width, logo.height));
		batch.draw_sprite(glm::vec2(logo.width * 1.1, 0.0),
			glm::vec4(logo.width / 2, 0, logo.width / 2, logo.height / 2)
		);
		batch.end();

		auto pen = glm::vec2(.0f, rstate.viewport.w);
		text.begin(ctx->get_system_font(), ctx->style_normal);
		pen = text.write(pen, 0, "float aspect = logo.get_aspect_ratio();");

		pen = text.write(pen, 0, "Hello World!\n");
		pen = text.write(pen, &ctx->style_error, "How is this?\n");
		pen = text.write(pen, &ctx->style_info, "Maybe this?\n");

		pen = text.write(pen, 0, "delta: %f\n", ctx->main_clock.delta());

		pen.x = 0;
		pen = text.write(pen, 0, "the quick brown fox jumps over lazy dog.\n");
		pen = text.write(pen, 0, "THE QUICK BROWN FOX JUMPS OVER LAZY DOG.\n");
		pen = text.write(pen, 0,
			"struct Texture\n"
			"{\n"
			"\tu32 gl_id;\n"
			"\ti32 width;\n"
			"\ti32 height;\n"
			"\tPixelFormat format;\n"
			"};\n"
		);
		pen = text.write(pen, 0, "1234567890|`~!@#$%^&*_+-=,./\\;':\"(){}[]<>\n");
		pen = text.write(pen, &ctx->style_info, "Press Alt F4 to quit.");

		pen = glm::vec2(rstate.viewport.z / 2, rstate.viewport.w);
		pen = text.write(pen, 0, "%s:\n", file);
		pen = text.write(pen, 0, code);
		text.end();
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