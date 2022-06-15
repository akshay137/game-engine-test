#include "text_renderer.hpp"
#include "../memory/memory.hpp"
#include "../logger.hpp"

#include <cstdarg>
#include <glad/glad.h>

namespace uhero::gfx
{
	Result TextRenderer::create(u32 max_glyphs,
		const char* vs, const char* fs
	)
	{
		if (nullptr == vs) vs = DEFAULT_TEXT_VERTEX_SHADER;
		if (nullptr == fs) fs = DEFAULT_TEXT_FRAGMENT_SHADER;

		VertexLayout layout;
		layout.add_attribute(VertexAttribute::Vec2);
		layout.add_attribute(VertexAttribute::Vec2);
		layout.add_attribute(VertexAttribute::ByteNVec4);
		layout.add_attribute(VertexAttribute::ByteNVec4);
		layout.add_attribute(VertexAttribute::Vec4);

		auto res = pso.create(layout, vs, fs);
		if (Result::Success != res)
		{
			UH_ERROR("Failed to create text renderer\n");
			assert(false);
			return res;
		}

		quads = UH_ALLOCATE_TYPE(GlyphQuad, max_glyphs);
		vertices = UH_ALLOCATE_TYPE(GlyphVertex, max_glyphs * 4);

		/*
			0_____1
			|    |
			2____|3
		*/
		u32 index_count = max_glyphs * 6;
		u16* indices = UH_STACK_ALLOCATE_TYPE(u16, index_count);
		constexpr u16 QUAD_INDICES[] = { 0, 1, 2, 2, 1, 3 };
		for (u32 i = 0; i < max_glyphs; i++)
		{
			for (u32 j = 0; j < 6; j++)
				indices[i * 6 + j] = i * 4 + QUAD_INDICES[j];
		}

		vertex_buffer.create(BufferType::Dynaminc, max_glyphs * 4);
		index_buffer.create(BufferType::Static, index_count, indices);

		this->max_glyphs = max_glyphs;

		return Result::Success;
	}

	void TextRenderer::clear()
	{
		UH_FREE(quads);
		UH_FREE(vertices);

		pso.clear();
		vertex_buffer.clear();
		index_buffer.clear();
	}

	void TextRenderer::begin(const Font& font, const FontStyle& style)
	{
		current_font = &font;
		current_style = style;

		current_glyphs = 0;
	}

	void TextRenderer::end()
	{
		if (0 == current_glyphs) return;

		update_vertex_buffer();

		current_font->atlas.bind_slot(1);
		pso.make_current();
		pso.set_vertex_buffer(vertex_buffer, 0, 0, vertex_buffer.stride());
		pso.set_index_buffer(index_buffer);

		u32 count = current_glyphs * 6;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

		current_glyphs = 0;
	}

	void TextRenderer::draw_glyph(const Glyph& glyph, glm::vec2 position,
		const FontStyle& style
	)
	{
		assert(quads);

		const Texture& tex = current_font->atlas;
		float scale = style.size / (float)current_font->line_height;

		GlyphQuad quad {};
		quad.glyph = glyph;
		quad.position = position;
		quad.text_color = style.text_color;
		quad.border_color = style.border_color;
		quad.size = glm::vec2(glyph.size_x, glyph.size_y) * scale;
		quad.border_size = style.border_size;
		quad.clip = glm::vec4(
			tex.normalized_x(glyph.pos_x), tex.normalized_y(glyph.pos_y),
			tex.normalized_x(glyph.size_x), tex.normalized_y(glyph.size_y)
		);

		quads[current_glyphs++] = quad;
		if (current_glyphs >= max_glyphs)
			this->end();
	}

	glm::vec2 TextRenderer::write(glm::vec2 position, const FontStyle* style,
		const char* fmt, ...
	)
	{
		UH_STACK_GROUP();
		usize buffer_size = Memory::kilobytes_to_bytes(1);
		char* buffer = UH_STACK_ALLOCATE_TYPE(char, buffer_size);
		va_list args;
		va_start(args, fmt);
		i32 count = vsnprintf(buffer, buffer_size, fmt, args);
		va_end(args);

		if (nullptr == style)
			style = &current_style;
		
		glm::vec2 pen = position;
		float scale = current_font->size_scale(style->size);
		float line_height = current_font->line_height + style->line_spacing;
		i32 line = 1;

		for (i32 i = 0; i < count; i++)
		{
			if (style->wrap_width > 0)
			{
				if (style->wrap_width < (pen.x - position.x)) // went beyond
				{
					pen.x = position.x;
					line++;
				}
			}

			pen.y = position.y - line * line_height * scale;
			char c = buffer[i];

			if (' ' == c)
			{
				pen.x += current_font->space * scale;
				continue;
			}
			if ('\n' == c)
			{
				pen.x = position.x;
				line++;
				continue;
			}
			if ('\t' == c)
			{
				pen.x += 4 * current_font->space * scale;
				continue;
			}

			const auto& g = current_font->find_glyph(c);

			float x = pen.x;
			pen.y -= (g.size_y - g.bearing_y) * scale;
			pen.x += (g.bearing_x) * scale;
			this->draw_glyph(g, pen, *style);

			pen.x = x + g.advance_x * scale;
		}

		pen.y = position.y - (line - 1) * line_height * scale;
		return pen;
	}

	glm::vec2 TextRenderer::calculate_size(i32 count, const char* buffer,
		const Font& font, const FontStyle& style
	)
	{
		glm::vec2 size(0.0f);
		float width = 0.0f;
		float scale = font.size_scale(style.size);
		float line_height = font.line_height + style.line_spacing;
		float space = font.space;
		i32 line = 1;

		for (i32 i = 0; i < count; i++)
		{
			char c = buffer[i];

			if ('\n' == c)
			{
				line++;
				if (size.x < width)
					size.x = width;
				width = 0.0f;
				continue;
			}
			if (' ' == c)
			{
				width += space * scale;
				continue;
			}
			if ('\t' == c)
			{
				width += (4 * space) * scale;
				continue;
			}

			const auto& glyph = font.find_glyph(c);

			width += glyph.advance_x * scale;
		}
		if (size.x < width) size.x = width;
		size.y = line * line_height * scale;
		return size;
	}

	void TextRenderer::update_vertex_buffer()
	{
		for (u32 i = 0; i < current_glyphs; i++)
		{
			const GlyphQuad& quad = quads[i];
			const GlyphVertex vert = vertex_glyph_from_quad(quad);
			const glm::vec4 clip = quad.clip;
			const glm::vec2 pos = quad.position;
			const glm::vec2 size = quad.size;

			GlyphVertex v[4] = { vert, vert, vert, vert };

			v[0].position = pos;
			v[0].uv = glm::vec2(clip.x, clip.y + clip.w);

			v[1].position = glm::vec2(pos.x + size.x, pos.y);
			v[1].uv = glm::vec2(clip.x + clip.z, clip.y + clip.w);

			v[2].position = glm::vec2(pos.x, pos.y + size.y);
			v[2].uv = glm::vec2(clip.x, clip.y);

			v[3].position = glm::vec2(pos.x + size.x, pos.y + size.y);
			v[3].uv = glm::vec2(clip.x + clip.z, clip.y);

			u32 vindex = i * 4;
			vertices[vindex + 0] = v[0];
			vertices[vindex + 1] = v[1];
			vertices[vindex + 2] = v[2];
			vertices[vindex + 3] = v[3];
		}

		vertex_buffer.update(vertices);
	}

	GlyphVertex TextRenderer::vertex_glyph_from_quad(const GlyphQuad& quad)
	{
		float width = 0.45f;
		float edge = 0.12f;

		GlyphVertex vertex {};
		vertex.text_color = quad.text_color;
		vertex.border_color = quad.border_color;
		vertex.width = width;
		vertex.edge = edge;
		vertex.border_width = width + quad.border_size;
		vertex.border_edge = edge;
		return vertex;
	}
}