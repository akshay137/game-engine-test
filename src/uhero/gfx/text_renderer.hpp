#ifndef UHERO_GFX_TEXT_RENDERER_H__
#define UHERO_GFX_TEXT_RENDERER_H__ 1

#include "../common.hpp"
#include "font.hpp"
#include "pso.hpp"
#include "buffer.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace uhero::gfx
{
	struct GlyphQuad
	{
		Glyph glyph;
		glm::vec2 position;
		glm::vec2 size;
		glm::vec4 clip;
		Color32 text_color;
		Color32 border_color;
		float border_size;
	};

	struct GlyphVertex
	{
		glm::vec2 position;
		glm::vec2 uv;
		Color32 text_color;
		Color32 border_color;
		float width;
		float edge;
		float border_width;
		float border_edge;
	};

	constexpr const char* DEFAULT_TEXT_VERTEX_SHADER = "assets/font.vert";
	constexpr const char* DEFAULT_TEXT_FRAGMENT_SHADER = "assets/font.frag";

	struct TextRenderer
	{
		PSO pso;
		TBuffer<GlyphVertex> vertex_buffer;
		TBuffer<u16> index_buffer;

		u32 max_glyphs;
		u32 current_glyphs;
		GlyphQuad* quads;
		GlyphVertex* vertices;

		const Font* current_font;
		FontStyle current_style;

		Result create(u32 max_glyphs,
			const char* vs=nullptr, const char* fs=nullptr
		);
		void clear();

		void begin(const Font& font, const FontStyle& style);
		void end();

		void draw_glyph(const Glyph& glyph, glm::vec2 position,
			const FontStyle& style
		);

		glm::vec2 write(glm::vec2 position, const FontStyle* style,
			const char* fmt, ...
		);

		void update_vertex_buffer();

		static GlyphVertex vertex_glyph_from_quad(const GlyphQuad& quad);
	};
}

#endif