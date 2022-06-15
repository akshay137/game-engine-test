#include "renderer.hpp"
#include "../memory/memory.hpp"
#include "../logger.hpp"

#include <cstdarg>
#include <glad/glad.h>

namespace uhero::gfx
{
	Result Renderer::create(u32 max_quads,
		const char* vs, const char* fs
	)
	{
		UH_STACK_GROUP();

		if (nullptr == vs) vs = REN_VERTEX_SHADER;
		if (nullptr == fs) fs = REN_FRAGMENT_SHADER;

		VertexLayout layout;
		layout.add_attribute(VertexAttribute::Vec2);
		layout.add_attribute(VertexAttribute::Vec2);
		layout.add_attribute(VertexAttribute::ByteNVec4);
		layout.add_attribute(VertexAttribute::Vec4);
		layout.add_attribute(VertexAttribute::ByteNVec4);

		auto res = pso.create(layout, vs, fs);
		if (Result::Success != res)
		{
			return res;
		}

		quads = UH_ALLOCATE_TYPE(Quad, max_quads);
		vertices = UH_ALLOCATE_TYPE(Vertex, max_quads * 4);

		u32 index_count = max_quads * 6;
		u16* indices = UH_STACK_ALLOCATE_TYPE(u16, index_count);
		constexpr u16 QUADI[] = { 0, 1, 2, 2, 1, 3 };
		for (u32 i = 0; i < index_count; i++)
		{
			for (u32 j = 0; j < 6; j++)
				indices[i * 6 + j] = i * 4 + QUADI[j];
		}

		vertex_buffer.create(BufferType::Dynaminc, max_quads * 4);
		element_buffer.create(BufferType::Static, index_count, indices);

		this->max_quads = max_quads;
		current_quads = 0;

		return Result::Success;
	}

	void Renderer::clear()
	{
		UH_FREE(quads);
		UH_FREE(vertices);

		pso.clear();
		vertex_buffer.clear();
		element_buffer.clear();
	}

	void Renderer::flush()
	{
		if (0 == current_quads) return;

		update_vertex_buffer();

		pso.make_current();
		pso.set_vertex_buffer(vertex_buffer, 0, 0, vertex_buffer.stride());
		pso.set_index_buffer(element_buffer);

		// draw
		u32 drawn = 0;
		while (drawn < current_quads)
		{
			u32 count = 0;
			const QuadType type = quads[drawn].type;
			const Texture* texture = quads[drawn].texture;

			while (type == quads[drawn + count].type)
			{
				count += 1;
			}

			// draw command here
			if (QuadType::SDFGlyph == type)
			{
				// call text code
				texture->bind_slot(1);
				pso.set_float(0, 1);

			}
			else
			{
				// call sprite/color code
				if (texture)
					texture->bind_slot(0);
				pso.set_float(0, 0);
			}
			usize index_offset = drawn * 6 * sizeof(u16);
			u32 index_count = count * 6;
			glDrawElements(GL_TRIANGLES, index_count,
				GL_UNSIGNED_SHORT, (void*)index_offset
			);

			drawn += count;
		}

		current_quads = 0;
	}

	void Renderer::draw_sprite(glm::vec2 pos, const Sprite& sprite,
		float scale, float angle,
		float blend_factor,
		Color32 color_key
	)
	{
		Quad quad {};
		quad.type = QuadType::Sprite;
		quad.texture = &sprite.texture;

		quad.rect = glm::vec4(pos.x, pos.y,
			sprite.src.z * scale, sprite.src.w * scale
		);
		quad.clip = glm::vec4(
			sprite.texture.normalized_x(sprite.src.x),
			sprite.texture.normalized_y(sprite.src.y),
			sprite.texture.normalized_x(sprite.src.z),
			sprite.texture.normalized_y(sprite.src.w)
		);
		quad.sprite.color = color_key;
		quad.sprite.blend = blend_factor;
		quad.sprite.angle = angle;

		quads[current_quads++] = quad;
		if (current_quads >= max_quads)
			this->flush();
	}

	void Renderer::draw_color(glm::vec2 pos, glm::vec2 size,
		Color32 color, float angle
	)
	{
		Quad quad {};
		quad.type = QuadType::Color;
		quad.texture = nullptr;

		quad.rect = glm::vec4(pos.x, pos.y, size.x, size.y);
		quad.clip = glm::vec4(0);
		quad.sprite.color = color;
		quad.sprite.blend = 1.0f;
		quad.sprite.angle = angle;

		quads[current_quads++] = quad;
		if (current_quads >= max_quads)
			this->flush();
	}

	void Renderer::draw_glyph(const Glyph& glyph, glm::vec2 pos,
		const Font& font, const FontStyle& style
	)
	{
		Quad quad {};
		quad.type = QuadType::SDFGlyph;
		quad.texture = &font.atlas;

		const Texture& tex = font.atlas;
		float scale = font.size_scale(style.size);

		quad.rect = glm::vec4(pos.x, pos.y,
			glyph.size_x * scale,
			glyph.size_y * scale
		);
		quad.clip = glm::vec4(
			tex.normalized_x(glyph.pos_x),
			tex.normalized_y(glyph.pos_y),
			tex.normalized_x(glyph.size_x),
			tex.normalized_y(glyph.size_y)
		);
		quad.glyph.text_color = style.text_color;
		quad.glyph.border_color = style.text_color;
		quad.glyph.border_size = style.border_size;

		quads[current_quads++] = quad;
		if (current_quads >= max_quads)
			this->flush();
	}

	glm::vec2 Renderer::write(glm::vec2 position,
		const Font& font, const FontStyle& style,
		const char* fmt, ...
	)
	{
		UH_STACK_GROUP();
		usize buffer_size = Memory::kilobytes_to_bytes(512);
		char* buffer = UH_STACK_ALLOCATE_TYPE(char, buffer_size);
		va_list args;
		va_start(args, fmt);
		i32 count = vsnprintf(buffer, buffer_size, fmt, args);
		va_end(args);

		glm::vec2 pen = position;
		float scale = font.size_scale(style.size);
		float line_height = font.line_height + style.line_spacing;
		i32 line = 1;

		for (i32 i = 0; i < count; i++)
		{
			// add wrapping support later

			char c = buffer[i];
			if ('\n' == c)
			{
				pen.x = position.x;
				line++;
				continue;
			}
			if (' ' == c)
			{
				pen.x += font.space * scale;
				continue;
			}
			if ('\t' == c)
			{
				pen.x += 4 * font.space * scale;
				continue;
			}

			const auto& g = font.find_glyph(c);

			float x = pen.x;
			pen.x += (g.bearing_x) * scale;
			pen.y = position.y - line * line_height * scale;
			pen.y -= (g.size_y - g.bearing_y) * scale;

			draw_glyph(g, pen, font, style);

			pen.x = x + g.advance_x * scale;
		}

		pen.y = position.y - (line - 1) * line_height * scale;
		return pen;
	}

	void Renderer::update_vertex_buffer()
	{
		if (0 == current_quads) return;

		// generate vertices
		for (u32 i = 0; i < current_quads; i++)
		{
			const Quad& quad = quads[i];
			const glm::vec4 clip = quad.clip;
			const glm::vec4 rect = quad.rect;
			Vertex temp;
			if (QuadType::SDFGlyph == quad.type)
				temp = glyph_vertex(quad);
			else
				temp = sprite_vertex(quad);
			
			Vertex v[4] = { temp, temp, temp, temp };

			v[0].position = glm::vec2(rect.x, rect.y);
			v[0].uv = glm::vec2(clip.x, clip.y + clip.w);

			v[1].position = glm::vec2(rect.x + rect.z, rect.y);
			v[1].uv = glm::vec2(clip.x + clip.z, clip.y + clip.w);

			v[2].position = glm::vec2(rect.x, rect.y + rect.w);
			v[2].uv = glm::vec2(clip.x, clip.y);

			v[3].position = glm::vec2(rect.x + rect.z, rect.y + rect.w);
			v[3].uv = glm::vec2(clip.x + clip.z, clip.y);

			u32 vindex = i * 4;
			vertices[vindex + 0] = v[0];
			vertices[vindex + 1] = v[1];
			vertices[vindex + 2] = v[2];
			vertices[vindex + 3] = v[3];
		}

		vertex_buffer.update(vertices);
	}

	Renderer::Vertex Renderer::glyph_vertex(const Quad& quad)
	{
		float width = 0.4f;
		float edge = 0.2f;

		Vertex v {};
		v.color = quad.glyph.text_color;
		v.glyph.border_color = quad.glyph.border_color;
		v.glyph.width = width;
		v.glyph.edge = edge;
		v.glyph.border_width = width + quad.glyph.border_size;
		v.glyph.border_edge = edge;
		return v;
	}

	Renderer::Vertex Renderer::sprite_vertex(const Quad& quad)
	{
		Vertex v {};
		v.color = quad.sprite.color;
		v.sprite.blend = quad.sprite.blend;
		return v;
	}
}