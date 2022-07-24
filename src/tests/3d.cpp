#include "3d.hpp"
#include "../uhero/logger.hpp"
#include "../uhero/res/font_atlas.hpp"
#include "../uhero/res/texture.hpp"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace uhero;

float factor = 0.5f;

auto rotate(glm::vec3 v, float angle, float x, float y, float z)
{
	auto res = glm::mat4(1);
	res = glm::rotate(res, angle, glm::vec3(x, y, z));
	auto rvec = res * glm::vec4(v.x, v.y, v.z, 0.0f);
	return glm::vec3(rvec.x, rvec.y, rvec.z);
};

namespace tests
{
	Result T3D::load(Context& ctx)
	{
		this->ctx = &ctx;
		ui.create(2048);
		gfx::RenderState rs;
		rs.resize(ctx.main_window.width, ctx.main_window.height);
		uirsbuffer.create(gfx::BufferType::Dynaminc, 1, &rs);

		font = res::load_font("assets/cascadia.atlas");
		// font = res::load_font("assets/firacode.atlas");
		// texture = res::load_texture("../won_miho_pt_01.png", 5);
		texture = res::load_texture("assets/logo.png", 3);
		texture.set_filter(gfx::TextureFilter::TriLinear);

		camera.position = glm::vec3(0, 0, 1);
		camera.forward = glm::vec3(0, 0, -1);
		rstate.viewport = { 0, 0, ctx.main_window.width, ctx.main_window.height };
		rstate.projection = glm::perspective(glm::radians(45.0f),
			rstate.viewport.w / rstate.viewport.z, 0.001f, 1000.0f
		);
		rstate.view = glm::lookAt(
			camera.position, camera.forward, glm::vec3(0, 1, 0)
		);
		rstate.time_data.system = 0;
		rstate.time_data.frame = 0;
		rsbuffer.create(gfx::BufferType::Dynaminc, 1, &rstate);

		std::vector<Vertex3D> verts = {
			Vertex3D(glm::vec3(0.5f, 0.5f, 0.0f)),
			Vertex3D(glm::vec3(0.5f, -0.5f, 0.0f)),
			Vertex3D(glm::vec3(-0.5f, -0.5f, 0.0f)),

			Vertex3D(glm::vec3(-0.5, 0.5f, 0.0f)),
			Vertex3D(glm::vec3(0.5f, 0.5f, 0.0f)),
			Vertex3D(glm::vec3(-0.5f, -0.5f, 0.0f)),
		};
		verts[0].color = { 1, 0, 0 };
		verts[1].color = { 0, 0, 1 };
		verts[2].color = { 0, 1, 0 };
		verts[3].color = { 0, 1, 1 };
		verts[4].color = { 1, 0, 0 };
		verts[5].color = { 0, 1, 0 };

		verts[0].uv = { 1, 0 };
		verts[1].uv = { 1, 1 };
		verts[2].uv = { 0, 1 };
		verts[3].uv = { 0, 0 };
		verts[4].uv = { 1, 0 };
		verts[5].uv = { 0, 1 };

		verts[0].normal = { 0.25, -0.25, 0 };
		verts[1].normal = { 0.25, -0.25, 0 };
		verts[2].normal = { 0.25, -0.25, 0 };
		verts[3].normal = { -0.25, 0.25, 0 };
		verts[4].normal = { -0.25, 0.25, 0 };
		verts[5].normal = { -0.25, 0.25, 0 };
		std::vector<uint16_t> inds = { 0, 1, 2, 3, 4, 5 };

		vbuffer.create(gfx::BufferType::Static, verts.size(), verts.data());
		ibuffer.create(gfx::BufferType::Static, inds.size(), inds.data());

		const char* vs = "assets/shaders/3d.vert";
		const char* fs = "assets/shaders/3d.frag";
		pso3d.create(Vertex3D::get_layout(), vs, fs,
			gfx::BlendState::OneMinusSrcAlpha,
			gfx::DepthState::LessEqual
		);

		UH_INFO("T3D loaded\n");
		return Result::Success;
	}

	void T3D::clear()
	{
		texture.clear();
		uirsbuffer.clear();
		rsbuffer.clear();
		pso3d.clear();
		vbuffer.clear();
		ibuffer.clear();
		font.clear();
		ui.clear();
	}

	void T3D::update(float delta)
	{
		auto& ip = ctx->input;
		if (ip.is_key_released(KeyCode::Escape))
		{
			ctx->request_exit();
		}

		float fdir = 0;
		float sdir = 0;
		float rot = 0;

		if (ip.is_key_down(KeyCode::W))
			fdir = 1;
		if (ip.is_key_down(KeyCode::S))
			fdir = -1;
		if (ip.is_key_down(KeyCode::A))
			sdir = -1;
		if (ip.is_key_down(KeyCode::D))
			sdir = 1;
		if (ip.is_key_down(KeyCode::ArrowRight))
			rot = -1;
		if (ip.is_key_down(KeyCode::ArrowLeft))
			rot = 1;
		
		if (ip.is_key_down(KeyCode::NP_MINUS))
			factor -= delta;
		if (ip.is_key_down(KeyCode::NP_PLUS))
			factor += delta;
		
		if (ip.is_key_released(KeyCode::R))
		{
			camera.position = glm::vec3(0, 0, 1);
			camera.forward = glm::vec3(0, 0, -1);
			factor = 0.5f;
		}

		camera.forward = rotate(camera.forward, rot * 2.5f * delta, 0, 1, 0);
		camera.position += camera.forward * fdir * 5.0f * delta;
		auto side = glm::cross(camera.forward, glm::vec3(0, 1, 0));
		camera.position += side * sdir * 5.0f * delta;
	}
	
	void T3D::render()
	{
		glm::vec2 screen = { ctx->main_window.width, ctx->main_window.height };
		rstate.viewport = { 0, 0, screen.x, screen.y };
		rstate.view = glm::lookAt(
			camera.position, camera.position + camera.forward, glm::vec3(0, 1, 0)
		);
		rstate.projection = glm::perspective(glm::radians(60.0f),
			screen.x / screen.y, 0.001f, 1000.0f
		);
		rstate.time_data.system = ctx->main_clock.seconds();
		rstate.time_data.frame = ctx->main_clock.delta();
		rsbuffer.update(&rstate);
		rsbuffer.bind_base(gfx::BufferBase::Uniform, 0);

		pso3d.make_current();
		pso3d.set_index_buffer(ibuffer);
		pso3d.set_vertex_buffer(vbuffer, 0, 0, vbuffer.stride());

		texture.bind_slot(0);
		
		for (auto i = 0; i < 16; i++)
		{
			float row = i / 4;
			float col = i % 4;
			float angle = 6.28f * (i / 16.0f);
			auto pos = rotate(glm::vec3(0, 0, -5), angle, 0, 1, 0);
			row = pos.x;
			col = pos.y;

			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, pos);
			model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
			glm::vec3 scale(1);
			scale.y = texture.get_aspect_ratio();
			model = glm::scale(model, scale);

			pso3d.set_mat4(0, glm::value_ptr(model));
			pso3d.set_float(1, 1);
			pso3d.draw_elements(gfx::TRIANGLES, ibuffer.count, 0);
		}

		//  debug
		ui.set_clip_size(screen.x, screen.y);
		uirsbuffer.bind_base(gfx::BufferBase::Uniform, 0);
		auto style = gfx::FontStyle(16 * 2);
		style.border_size = 0.001;
		auto delta = ctx->main_clock.delta();
		auto pen = glm::vec2(0, screen.y);
		pen = ui.write_format(pen, font, style, "delta: %.4f\n", delta);
		pen = ui.write_format(pen, font, style,
			"cam: p=[%.2f, %.2f, %.2f] | f=[%.2f, %.2f, %.2f]\n",
			camera.position.x, camera.position.y, camera.position.z,
			camera.forward.x, camera.forward.y, camera.forward.z
		);
		pen = ui.write_format(pen, font, style, "factor: %.3f\n", factor);
		ui.flush();
	}
}