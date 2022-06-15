#include "batch_renderer.hpp"
#include "../memory/memory.hpp"
#include "texture.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result BatchRenderer::create(u32 max_sprites,
		const char* vs, const char* fs
	)
	{
		if (nullptr == vs) vs = BATCH_VERTEX_SHADER;
		if (nullptr == fs) fs = BATCH_FRAGMENT_SHADER;

		VertexLayout layout {};
		layout.add_attribute(VertexAttribute::Vec2); // position
		layout.add_attribute(VertexAttribute::Vec3); // uv + blend
		layout.add_attribute(VertexAttribute::ByteNVec4); // color
		
		auto res = pso.create(layout, vs, fs);
		if (Result::Success != res)
		{
			return res;
		}

		quads = UH_ALLOCATE_TYPE(Quad, max_sprites);
		vertices = UH_ALLOCATE_TYPE(Vertex2d, max_sprites * 4);

		/*
			0_____1
			|    |
			2____|3
		*/
		u32 index_count = max_sprites * 6;
		u16* indices = UH_STACK_ALLOCATE_TYPE(u16, index_count);
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

		return Result::Success;
	}

	void BatchRenderer::clear()
	{
		UH_FREE(quads);
		UH_FREE(vertices);
		pso.clear();
		vertex_buffer.clear();
		index_buffer.clear();
	}

	void BatchRenderer::begin(const Texture& texture)
	{
		current_texture = &texture;
	}

	void BatchRenderer::end()
	{
		if (0 == current_quads) return;

		current_texture->bind_slot(0);
		update_vertex_buffer();

		pso.make_current();
		pso.set_vertex_buffer(vertex_buffer, 0, 0, vertex_buffer.stride());
		pso.set_index_buffer(index_buffer);

		u32 index_count = current_quads * 6;
		glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);

		current_quads = 0;
	}

	void BatchRenderer::draw_sprite(glm::vec2 position, glm::vec4 src,
		float scale, float angle, float blend_factor, Color32 color_key
	)
	{
		const Texture& tex = *current_texture;
		Quad quad {};
		quad.rect = glm::vec4(position.x, position.y, src.z * scale, src.w * scale);
		quad.clip = glm::vec4(
			tex.normalized_x(src.x),
			tex.normalized_y(src.y),
			tex.normalized_x(src.z),
			tex.normalized_y(src.w)
		);
		quad.color = color_key;
		quad.angle = angle;
		quad.blend = blend_factor;

		quads[current_quads++] = quad;

		if (current_quads >= max_quads)
			this->end();
	}

	void BatchRenderer::draw_color(glm::vec2 position, glm::vec2 size,
		Color32 color, float angle
	)
	{
		Quad quad {};
		quad.rect = glm::vec4(position.x, position.y, size.x, size.y);
		quad.clip = glm::vec4(0.0f);
		quad.color = color;
		quad.angle = angle;
		quad.blend = 1.0f;

		quads[current_quads++] = quad;
		if (current_quads >= max_quads)
			this->end();
	}

	void BatchRenderer::update_vertex_buffer()
	{
		for (u32 i = 0; i < current_quads; i++)
		{
			const Quad& quad = quads[i];

			Vertex2d v[4];
			v[0].position = glm::vec2(quad.rect.x, quad.rect.y);
			v[0].uv = glm::vec2(quad.clip.x, quad.clip.y + quad.clip.w);
			v[0].blend = quad.blend;
			v[0].color = quad.color;

			v[1].position = glm::vec2(quad.rect.x + quad.rect.z, quad.rect.y);
			v[1].uv = glm::vec2(quad.clip.x + quad.clip.z,
				quad.clip.y + quad.clip.w
			);
			v[1].blend = quad.blend;
			v[1].color = quad.color;

			v[2].position = glm::vec2(quad.rect.x, quad.rect.y + quad.rect.w);
			v[2].uv = glm::vec2(quad.clip.x, quad.clip.y);
			v[2].blend = quad.blend;
			v[2].color = quad.color;

			v[3].position = glm::vec2(quad.rect.x + quad.rect.z,
				quad.rect.y + quad.rect.w
			);
			v[3].uv = glm::vec2(quad.clip.x + quad.clip.z, quad.clip.y);
			v[3].blend = quad.blend;
			v[3].color = quad.color;

			u32 v_index = i * 4;
			vertices[v_index + 0] = v[0];
			vertices[v_index + 1] = v[1];
			vertices[v_index + 2] = v[2];
			vertices[v_index + 3] = v[3];
		}

		vertex_buffer.update(vertices);
	}
}