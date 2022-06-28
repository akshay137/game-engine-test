#include "renderer.hpp"
#include "../memory/memory.hpp"
#include "../logger.hpp"

#include <cstdarg>
#include <cmath>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace uhero::gfx
{
	Result Renderer::create(u32 max_quads,
		const char* vs, const char* fs
	)
	{
		UH_FRAME_STACK_GROUP();

		if (nullptr == vs) vs = REN_VERTEX_SHADER;
		if (nullptr == fs) fs = REN_FRAGMENT_SHADER;

		VertexLayout layout;
		layout.add_attribute(VertexAttribute::Vec2); // position
		layout.add_attribute(VertexAttribute::Vec2); // tex coords
		layout.add_attribute(VertexAttribute::Vec2); // normalized coords
		layout.add_attribute(VertexAttribute::ByteNVec4); // color
		layout.add_attribute(VertexAttribute::ByteNVec4); // blend | text data
		layout.add_attribute(VertexAttribute::ByteNVec4); // border color

		auto res = pso.create(layout, vs, fs);
		if (Result::Success != res)
		{
			return res;
		}

		quads = UH_ALLOCATE_TYPE(Quad, max_quads);
		vertices = UH_ALLOCATE_TYPE(Vertex, max_quads * 4);

		u32 index_count = max_quads * 6;
		u16* indices = UH_FRAME_STACK_ALLOCATE_TYPE(u16, index_count);
		/*
			0 1
			2 3
		*/
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
			const Quad& first = quads[drawn];
			const Texture* texture = first.texture;

			u32 count = 1;
			while (first.can_batch_together(quads[drawn + count]))
			{
				if (current_quads <= (drawn + count)) break;
				count += 1;
			}

			// draw command here
			if (QuadType::SDFGlyph == first.type)
			{
				texture->bind_slot(1);
				pso.set_int(UNIFORM_PROGRAM_MODE, UPM_GLYPH);
			}
			else if (QuadType::Sprite == first.type)
			{
				texture->bind_slot(0);
				pso.set_int(UNIFORM_PROGRAM_MODE, UPM_SPRITE);
			}
			else if (QuadType::Color == first.type)
			{
				pso.set_int(UNIFORM_PROGRAM_MODE, UPM_COLOR);
				Texture::reset_slot(0);
			}

			usize index_offset = drawn * 6 * sizeof(u16);
			u32 index_count = count * 6;
			pso.draw_elements(GL_TRIANGLES, index_count, (void*)index_offset);

			drawn += count;
		}

		current_quads = 0;
	}

	void Renderer::draw_texture(glm::vec2 pos, glm::vec2 size,
		const Texture& texture, glm::vec4 src,
		float angle, glm::vec2 center,
		float blend_factor, Color32 color_key,
		float circle
	)
	{
		Quad quad {};
		quad.type = QuadType::Sprite;
		quad.texture = &texture;

		quad.rect = glm::vec4(pos.x, pos.y, size.x, size.y);
		quad.clip = glm::vec4(
			texture.normalized_x(src.x),
			texture.normalized_y(src.y),
			texture.normalized_x(src.z),
			texture.normalized_y(src.w)
		);
		quad.center = center;
		quad.sprite.color = color_key;
		quad.sprite.blend = blend_factor;
		quad.sprite.angle = angle;
		quad.sprite.circle = circle;

		this->submit_quad(quad);
	}

	void Renderer::draw_color(glm::vec2 pos, glm::vec2 size,
		Color32 color, float angle, glm::vec2 center,
		float circle
	)
	{
		Quad quad {};
		quad.type = QuadType::Color;
		quad.texture = nullptr;

		quad.rect = glm::vec4(pos.x, pos.y, size.x, size.y);
		quad.clip = glm::vec4(0);
		quad.center = center;
		quad.sprite.color = color;
		quad.sprite.blend = 1.0f;
		quad.sprite.angle = angle;
		quad.sprite.circle = circle;

		this->submit_quad(quad);
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
		quad.glyph.border_color = style.border_color;
		quad.glyph.border_size = style.border_size;

		this->submit_quad(quad);
	}

	glm::vec2 Renderer::write(glm::vec2 position,
		const Font& font, const FontStyle& style,
		usize count, const char* buffer
	)
	{
		glm::vec2 pen = position;
		float scale = font.size_scale(style.size);
		float line_height = font.line_height + style.line_spacing;
		i32 line = 1;

		for (usize i = 0; i < count; i++)
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
				pen.x += font.tab_spaces * font.space * scale;
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

	glm::vec2 Renderer::write_format(glm::vec2 position,
		const Font& font, const FontStyle& style,
		const char* fmt, ...
	)
	{
		UH_FRAME_STACK_GROUP();
		usize buffer_size = Memory::kilobytes_to_bytes(512);
		char* buffer = UH_FRAME_STACK_ALLOCATE_TYPE(char, buffer_size);
		va_list args;
		va_start(args, fmt);
		i32 count = vsnprintf(buffer, buffer_size, fmt, args);
		va_end(args);

		return this->write(position, font, style, count, buffer);
	}

	void Renderer::submit_quad(const Quad& quad)
	{
		quads[current_quads++] = quad;
		if (current_quads >= max_quads)
			this->flush();
	}

	glm::vec2 transform_vertex(const glm::mat4& trans, const glm::vec2& vertex)
	{
		auto res = trans * glm::vec4(vertex, 0.0f, 1.0f);
		return glm::vec2(res.x, res.y);
	}

	void Renderer::update_vertex_buffer()
	{
		if (0 == current_quads) return;

		// generate vertices
		for (u32 i = 0; i < current_quads; i++)
		{
			const Quad& quad = quads[i];
			constexpr glm::vec2 sprite_positions[4] = {
				{ -.5f, .5f }, // top left
				{ .5f, .5f }, // top right
				{ -.5f, -.5f }, // bottom left
				{ .5f, -.5f }, // bottom right
			};
			constexpr glm::vec2 glyph_positions[4] = {
				{ 0, 1 }, // top left
				{ 1, 1 }, // top right
				{ 0, 0 }, // bottom left
				{ 1, 0 }, // bottom right
			};
			constexpr glm::vec2 coords[4] = {
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 },
			};
			const glm::vec2* positions = sprite_positions;

			Vertex temp;
			glm::mat4 uv = glm::mat4(1);
			uv = glm::translate(uv, glm::vec3(quad.clip.x, quad.clip.y, 0.0f));
			uv = glm::scale(uv, glm::vec3(quad.clip.z, quad.clip.w, 1.0f));
			
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(quad.rect.x, quad.rect.y, .0f));
			model = glm::scale(model, glm::vec3(quad.rect.z, quad.rect.w, 1.0f));
			
			if (QuadType::SDFGlyph == quad.type)
			{
				positions = glyph_positions;
				temp = glyph_vertex(quad);
			}
			else
			{
				temp = sprite_vertex(quad);
				model = glm::rotate(model, quad.sprite.angle, glm::vec3(0, 0, 1));
			}
			
			Vertex v[4] = { temp, temp, temp, temp };
			for (auto i = 0; i < 4; i++)
			{
				v[i].position = transform_vertex(model, positions[i]);
				v[i].uv = transform_vertex(uv, coords[i]);
				v[i].quad_uv = coords[i];
			}

			u32 vindex = i * 4;
			vertices[vindex + 0] = v[0];
			vertices[vindex + 1] = v[1];
			vertices[vindex + 2] = v[2];
			vertices[vindex + 3] = v[3];
		}

		vertex_buffer.update_range(0, current_quads * 4, vertices);
	}

	Renderer::Vertex Renderer::glyph_vertex(const Quad& quad)
	{
		// TODO: calculate this based on size of text
		float width = 0.5f;
		float edge = 0.1f;

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
		v.sprite.circle = quad.sprite.circle;
		return v;
	}
}