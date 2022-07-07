#ifndef UHERO_GFX_RENDERER_H__
#define UHERO_GFX_RENDERER_H__ 1

#include "../common.hpp"
#include "font.hpp"
#include "pso.hpp"
#include "buffer.hpp"
#include "sprite.hpp"
#include "../math/norm_type.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace uhero::gfx
{
	constexpr const char* REN_VERTEX_SHADER = "assets/gfx.vert";
	constexpr const char* REN_FRAGMENT_SHADER = "assets/gfx.frag";

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
					float circle;
				} sprite;

				struct {
					Color32 text_color;
					Color32 border_color;
					float border_size;
				} glyph;
			};

			bool can_batch_together(const Quad& other) const
			{
				if (type != other.type) return false;
				if (QuadType::Color != type)
				{
					if (texture != other.texture) return false;
				}

				return true;
			}
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
			glm::vec2 quad_uv; // 0...1
			Color32 color;
			union
			{
				struct {
					math::Normalized<u8> blend;
					math::Normalized<u8> circle;
				} sprite;
				struct {
					math::Normalized<u8> width;
					math::Normalized<u8> edge;
					math::Normalized<u8> border_width;
					math::Normalized<u8> border_edge;
					Color32 border_color;
				} glyph;
			};
		}; // struct vertex

		static_assert(sizeof(Vertex) == 9 * sizeof(f32), "malformed vertex");

		constexpr static i32 UNIFORM_PROGRAM_MODE = 0;
		constexpr static i32 UPM_COLOR = 0;
		constexpr static i32 UPM_SPRITE = 1;
		constexpr static i32 UPM_GLYPH = 2;
		constexpr static u32 MAX_IND = (u16)~0;

		u32 max_quads;
		u32 current_quads;
		glm::vec2 clip_size; // for out of bounds clipping

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

		void set_clip_size(float w, float h)
		{
			clip_size = { w, h };
		}

		// main draw texture function
		void draw_texture(glm::vec2 pos, glm::vec2 size,
			const Texture& texture, glm::vec4 src,
			float angle,
			float blend_factor, Color32 color_key,
			bool is_circle
		);
		void draw_color(glm::vec2 pos, glm::vec2 size,
			Color32 color, float angle=0.0f,
			bool is_circle=false
		);
		void draw_circle(glm::vec2 pos, float radius, Color32 color)
		{
			draw_color(pos, glm::vec2(radius * 2), color, 0, 1);
		}

		void draw_glyph(const Glyph& glyph, glm::vec2 pos,
			const Font& font, const FontStyle& style
		);
		glm::vec2 write(glm::vec2 position,
			const Font& font, const FontStyle& style,
			usize count, const char* buffer
		);
		glm::vec2 write_format(glm::vec2 position,
			const Font& font, const FontStyle& style,
			const char* fmt, ...
		);
		void submit_quad(const Quad& quad);

		// draws complete texture with 1:1 scale
		void draw_texture(glm::vec2 pos, const Texture& texture, bool is_circle=false)
		{
			glm::vec4 src(0, 0, texture.width, texture.height);
			glm::vec2 size(src.z, src.w);
			draw_texture(pos, size, texture, src, 0, 1, Color32(255), is_circle);
		}
		// draws complete texture with 1:1 scale
		void draw_texture(glm::vec2 pos, glm::vec2 size, const Texture& texture,
			bool is_circle=false
		)
		{
			glm::vec4 src(0, 0, texture.width, texture.height);
			draw_texture(pos, size, texture, src, 0, 1, Color32(255), is_circle);
		}
		// draws a region of texture with given size
		void draw_texture(glm::vec2 pos, glm::vec2 size,
			const Texture& texture, glm::vec4 src,
			bool is_circle=false
		)
		{
			draw_texture(pos, size, texture, src, 0, 1, Color32(255), is_circle);
		}

		// draws a rotated texture
		void draw_texture(glm::vec2 pos, glm::vec2 size, const Texture& texture,
			float angle, bool is_circle=false
		)
		{
			glm::vec4 src(0, 0, texture.width, texture.height);
			draw_texture(pos, size, texture, src, angle, 1, Color32(255), is_circle);
		}

		// draws region of texture in rotated quad
		void draw_texture(glm::vec2 pos, glm::vec2 size,
			const Texture& texture, glm::vec4 src,
			float angle, bool is_circle=false
		)
		{
			draw_texture(pos, size, texture, src, angle, 1, Color32(255), is_circle);
		}

		// draws a color multiplied texture
		void draw_texture(glm::vec2 pos, glm::vec2 size, const Texture& texture,
			float blend_factor, Color32 color_key, bool is_circle=false
		)
		{
			glm::vec4 src(0, 0, texture.width, texture.height);
			draw_texture(pos, size, texture, src, 0,
				blend_factor, color_key, is_circle
			);
		}

		// draws a region of texture with color multiplied
		void draw_texture(glm::vec2 pos, glm::vec2 size,
			const Texture& texture, glm::vec4 src,
			float blend_factor, Color32 color_key,
			bool is_circle=false
		)
		{
			draw_texture(pos, size, texture, src, 0,
				blend_factor, color_key, is_circle
			);
		}

		void update_vertex_buffer();
		Vertex glyph_vertex(const Quad& quad);
		Vertex sprite_vertex(const Quad& quad);
	};
}

#endif