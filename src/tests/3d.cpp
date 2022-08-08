#include "3d.hpp"
#include "../uhero/logger.hpp"
#include "../uhero/res/font_atlas.hpp"
#include "../uhero/res/texture.hpp"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace uhero;

float factor = 0.01f;

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
		texture = res::load_texture("../won_miho_864.png", 3);
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

		// 0 1
		// 2 3
		PieVertex tl, tr;
		PieVertex bl, br;
		tl.position = { -0.5, 0.5, 0 };
		tl.uv = { 0, 0 };
		tr.position = { 0.5, 0.5, 0 };
		tr.uv = { 1, 0 };
		bl.position = { -0.5, -0.5, 0 };
		bl.uv = { 0, 1 };
		br.position = { 0.5, -0.5, 0 };
		br.uv = { 1, 1 };

		vertices.push_back(tl);
		vertices.push_back(tr);
		vertices.push_back(bl);
		vertices.push_back(br);
		std::vector<uint16_t> inds = { 0, 1, 2, 2, 1, 3 };

		vbuffer.create(gfx::BufferType::Dynaminc, vertices.size(), vertices.data());
		ibuffer.create(gfx::BufferType::Static, inds.size(), inds.data());

		gfx::VertexLayout layout;
		layout.add_attribute(gfx::VertexAttribute::Vec3); // position
		layout.add_attribute(gfx::VertexAttribute::Vec2); // tex_coords
		layout.add_attribute(gfx::VertexAttribute::Vec2); // uv
		layout.add_attribute(gfx::VertexAttribute::Vec2); // angles

		const char* vs = "assets/shaders/pie.vert";
		const char* fs = "assets/shaders/pie.frag";
		pso_pie.create(layout, vs, fs,
			gfx::BlendState::OneMinusSrcAlpha,
			gfx::DepthState::LessEqual
		);

		pie_count = 3;

		UH_INFO("T3D loaded\n");
		return Result::Success;
	}

	void T3D::clear()
	{
		texture.clear();
		uirsbuffer.clear();
		rsbuffer.clear();
		pso_pie.clear();
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
		
		if (ip.is_key_released(KeyCode::NP_MINUS))
			pie_count -= 1;
		if (ip.is_key_released(KeyCode::NP_PLUS))
			pie_count += 1;
		
		if (ip.is_key_down(KeyCode::Z))
			factor -= delta;
		if (ip.is_key_down(KeyCode::X))
			factor += delta;
		
		if (ip.is_key_released(KeyCode::R))
		{
			camera.position = glm::vec3(0, 0, 1);
			camera.forward = glm::vec3(0, 0, -1);
			factor = 0.01f;
			pie_count = 3;
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

		ctx->gfx.clear_buffer(gfx::CYAN);
		pso_pie.make_current();
		pso_pie.set_index_buffer(ibuffer);
		pso_pie.set_vertex_buffer(vbuffer, 0, 0, vbuffer.stride());

		texture.bind_slot(0);

		float pie_angle = 6.28f / pie_count;
		float half_angle = pie_angle * 0.5f;
		float half_width = glm::sin(half_angle);
		float ratio = half_width;
		if (1 == (int)pie_count)
			ratio = 1;
		for (auto i = 0; i < (int)pie_count; i++)
		{
			float angle = pie_angle * i;
			auto pos = rotate(glm::vec3(0, 0.25 + factor, 0), angle, 0, 0, 1);

			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, pos);
			model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
			glm::vec3 scale = { ratio, 0.5, 1 };
			model = glm::scale(model, scale);

			float xoffset = (1 - ratio) * 0.5;
			pos = glm::vec3(xoffset, 0, 0);
			scale = glm::vec3(ratio, 0.5, 1);

			glm::mat4 uv = glm::mat4(1);
			// rotate around texture center
			uv = glm::translate(uv, glm::vec3(0.5, 0.5, 0));
			uv = glm::rotate(uv, -angle, glm::vec3(0, 0, 1));
			uv = glm::translate(uv, glm::vec3(-0.5, -0.5, 0));

			uv = glm::translate(uv, pos);
			uv = glm::scale(uv, scale);

			pso_pie.set_mat4(0, glm::value_ptr(model));
			pso_pie.set_float(1, angle);
			pso_pie.set_float(2, pie_angle);
			pso_pie.set_mat4(3, glm::value_ptr(uv));
			pso_pie.set_float(4, ratio);
			pso_pie.draw_elements(gfx::TRIANGLES, ibuffer.count, 0);
		}

		//  debug
		ui.set_clip_size(screen.x, screen.y);
		uirsbuffer.bind_base(gfx::BufferBase::Uniform, 0);
		
		glm::vec2 size = { 256, 256 * texture.get_aspect_ratio() };
		ui.draw_texture(size * 0.5f, size, texture);

		auto style = gfx::FontStyle(16);
		style.text_color = gfx::BLACK;
		style.border_size = 0.001;
		style.border_color = gfx::BLACK;
		auto delta = ctx->main_clock.delta();
		auto pen = glm::vec2(0, screen.y);
		pen = ui.write_format(pen, font, style, "delta: %.4f\n", delta);
		pen = ui.write_format(pen, font, style,
			"cam: p=[%.2f, %.2f, %.2f] | f=[%.2f, %.2f, %.2f]\n",
			camera.position.x, camera.position.y, camera.position.z,
			camera.forward.x, camera.forward.y, camera.forward.z
		);
		pen = ui.write_format(pen, font, style,
			"pie : { count: %.3f, angle: %.3f, ratio: %.3f }\n",
			pie_count, glm::degrees(pie_angle), ratio
		);
		pen = ui.write_format(pen, font, style,
			"(1 - ratio) * 0.5 = %f\n", (1 - ratio) * 0.5
		);
		ui.flush();
	}
}