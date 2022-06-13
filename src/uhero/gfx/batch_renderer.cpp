#include "batch_renderer.hpp"
#include "../memory/memory.hpp"
#include "texture.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result BatchRenderer::create(u32 max_sprites, const char* vs, const char* fs)
	{
		if (nullptr == vs) vs = BATCH_VERTEX_SHADER;
		if (nullptr == fs) fs = BATCH_FRAGMENT_SHADER;

		VertexLayout layout {};
		layout.add_attribute(VertexAttribute::Vec2); // position
		layout.add_attribute(VertexAttribute::Vec2); // uv
		layout.add_attribute(VertexAttribute::Vec3); // color
		auto res = pso.create(layout, vs, fs);
		if (Result::Success != res)
		{
			return res;
		}

		// quads = new Quad[max_sprites];
		// vertices = new Vertex2d[max_sprites * 4];
		quads = UH_ALLOCATE_TYPE(Quad, max_sprites);
		vertices = UH_ALLOCATE_TYPE(Vertex2d, max_sprites * 4);

		/*
			0_____1
			|    |
			2____|3
		*/
		u32 index_count = max_sprites * 6;
		// u16* indices = new u16[index_count];
		u16* indices = UH_ALLOCATE_TYPE(u16, index_count);
		constexpr u16 QUAD_INDICES[] = { 0, 1, 2, 2, 1, 3 };
		for (u32 i = 0; i < max_sprites; i++)
		{
			for (u32 j = 0; j < 6; j++)
				indices[i * 6 + j] = i * 4 + QUAD_INDICES[j];
		}

		vertex_buffer.create(BufferType::Dynaminc, max_sprites * 4);
		index_buffer.create(BufferType::Static, index_count, indices);

		current_quads = 0;
		this->max_quads = max_sprites;
		current_texture = nullptr;

		UH_FREE(indices);
		// delete[] indices;
		return Result::Success;
	}

	void BatchRenderer::clear()
	{
		UH_FREE(quads);
		UH_FREE(vertices);
		// delete[] quads;
		// delete[] vertices;
		pso.clear();
		vertex_buffer.clear();
		index_buffer.clear();
	}

	void BatchRenderer::begin(const Texture& texture)
	{
		current_texture = &texture;
		texture.bind_slot(0);
	}

	void BatchRenderer::end()
	{
		update_vertex_buffer();

		pso.make_current();
		pso.set_vertex_buffer(vertex_buffer, 0, 0, vertex_buffer.stride());
		pso.set_index_buffer(index_buffer);

		u32 index_count = current_quads * 6;
		glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);

		current_texture = nullptr;
		current_quads = 0;
	}

	void BatchRenderer::draw_sprite(glm::vec2 position, glm::vec4 src, float scale)
	{
		Quad quad {};
		quad.rect = glm::vec4(position.x, position.y, src.z * scale, src.w * scale);
		quad.clip = glm::vec4(
			src.x / current_texture->width,
			src.y / current_texture->height,
			src.z / current_texture->width,
			src.w / current_texture->height
		);
		quad.color = Color32(1, 1, 1, 1);
		quad.angle = 1.0f;

		quads[current_quads++] = quad;

		if (current_quads >= max_quads)
			this->end();
	}

	void BatchRenderer::update_vertex_buffer()
	{
		glm::vec3 color;
		float alpha;
		for (u32 i = 0; i < current_quads; i++)
		{
			const Quad& quad = quads[i];
			quad.color.to_float(color.r, color.g, color.b, alpha);

			Vertex2d v[4];
			v[0].position = glm::vec2(quad.rect.x, quad.rect.y);
			v[0].uv = glm::vec2(quad.clip.x, quad.clip.y + quad.clip.w);
			v[0].color = color;

			v[1].position = glm::vec2(quad.rect.x + quad.rect.z, quad.rect.y);
			v[1].uv = glm::vec2(quad.clip.x + quad.clip.z,
				quad.clip.y + quad.clip.w
			);
			v[1].color = color;

			v[2].position = glm::vec2(quad.rect.x, quad.rect.y + quad.rect.w);
			v[2].uv = glm::vec2(quad.clip.x, quad.clip.y);
			v[2].color = color;

			v[3].position = glm::vec2(quad.rect.x + quad.rect.z,
				quad.rect.y + quad.rect.w
			);
			v[3].uv = glm::vec2(quad.clip.x + quad.clip.z, quad.clip.y);
			v[3].color = color;

			u32 v_index = i * 4;
			vertices[v_index + 0] = v[0];
			vertices[v_index + 1] = v[1];
			vertices[v_index + 2] = v[2];
			vertices[v_index + 3] = v[3];
		}

		vertex_buffer.update(vertices);
	}
}