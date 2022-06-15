#ifndef UHERO_GFX_RENDERER_H__
#define UHERO_GFX_RENDERER_H__ 1

#include "../common.hpp"
#include "font.hpp"
#include "pso.hpp"
#include "buffer.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace uhero::gfx
{
	constexpr const char* REN_VERTEX_SHADER = "assets/gfx.vert";
	constexpr const char* REN_FRAGMENT_SHADER = "assets/gfx.frag";

	struct Sprite
	{
		const Texture& texture;
		glm::vec4 src;

		Sprite(const Texture& texture)
			: texture{texture}
		{
			src = glm::vec4(0, 0, texture.width, texture.height);
		}

		Sprite(const Texture& texture, const glm::vec4& clip)
			: texture{texture}
		{
			src = clip;
		}
	};

	// uber renderer
	// can render texture sprites, colored blocks, SDF glyphs
	struct Renderer
	{
		enum class QuadType : u32
		{
			None = 0,
			Sprite,
			Color,
			SDFGlyph,
		};
		
		struct Quad
		{
			QuadType type;
			const Texture* texture;
			glm::vec4 rect;
			glm::vec4 clip;
			union {
				struct {
					Color32 color;
					float blend;
					float angle;
				} sprite;

				struct {
					Color32 text_color;
					Color32 border_color;
					float border_size;
				} glyph;
			};
		}; // struct Quad

		/*
		VertexLayout:
			Vec2
			Vec2
			ByteVec4Normalized
			Vec4
			ByteVec4Normalized
		*/
		struct Vertex
		{
			glm::vec2 position;
			glm::vec2 uv;
			Color32 color;
			union
			{
				struct {
					float blend;
				} sprite;
				struct {
					float width;
					float edge;
					float border_width;
					float border_edge;
					Color32 border_color;
				} glyph;
			};
		}; // struct vertex

		static_assert(sizeof(Vertex) == 10 * sizeof(f32), "malformed Vertex");

		u32 max_quads;
		u32 current_quads;

		PSO pso;
		TBuffer<Vertex> vertex_buffer;
		TBuffer<u16> element_buffer;

		Quad* quads;
		Vertex* vertices;

		Result create(u32 max_quads,
			const char* vs=nullptr, const char* fs=nullptr
		);
		void clear();

		void flush();

		void draw_sprite(glm::vec2 pos, const Sprite& sprite,
			float scale=1.0f, float angle=0.0f,
			float blend_factor=0.0f,
			Color32 color_key=Color32(255, 255, 255, 255)
		);
		void draw_color(glm::vec2 pos, glm::vec2 size,
			Color32 color, float angle=0.0f
		);

		void draw_glyph(const Glyph& glyph, glm::vec2 pos,
			const Font& font, const FontStyle& style
		);
		glm::vec2 write(glm::vec2 position,
			const Font& font, const FontStyle& style,
			const char* fmt, ...
		);

		void update_vertex_buffer();
		Vertex glyph_vertex(const Quad& quad);
		Vertex sprite_vertex(const Quad& quad);
	};
}

#endif