#ifndef TESTS_3D_H__
#define TESTS_3D_H__ 1

#include "../uhero/uhero.hpp"
#include "../uhero/application.hpp"

#include "../uhero/gfx/pso.hpp"
#include "../uhero/gfx/buffer.hpp"
#include "../uhero/gfx/renderer.hpp"
#include "../uhero/gfx/texture.hpp"

#include "vertex3d.hpp"
#include <glm/glm.hpp>

namespace tests
{
	struct TimeData
	{
		float system;
		float frame;
	};

	struct RState
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 viewport;
		TimeData time_data;
	};

	struct Camera
	{
		glm::vec3 position;
		glm::vec3 forward;
	};

	struct T3D : public uhero::IApplication
	{
		uhero::Context* ctx;
		uhero::gfx::Renderer ui;
		uhero::gfx::Font font;
		uhero::gfx::Texture texture;

		Camera camera;
		RState rstate;
		uhero::gfx::PSO pso3d;
		uhero::gfx::TBuffer<uint16_t> ibuffer;
		uhero::gfx::TBuffer<Vertex3D> vbuffer;
		uhero::gfx::TBuffer<RState> rsbuffer;
		uhero::gfx::TBuffer<uhero::gfx::RenderState> uirsbuffer;

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;
	};
}

#endif