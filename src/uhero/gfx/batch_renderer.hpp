#ifndef UHERO_GFX_BATCH_RENDERER_H__
#define UHERO_GFX_BATCH_RENDERER_H__ 1

#include "../common.hpp"
#include "pso.hpp"
#include "buffer.hpp"
#include "color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace uhero::gfx
{
	constexpr const char* BATCH_VERTEX_SHADER = "assets/quad.vert";
	constexpr const char* BATCH_FRAGMENT_SHADER = "assets/quad.frag";

	struct Quad
	{
		glm::vec4 rect;
		glm::vec4 clip; // texture region
		float angle;
		Color32 color;

		Quad() = default;
	};

	struct Vertex2d
	{
		glm::vec2 position;
		glm::vec2 uv;
		glm::vec3 color;
	};

	constexpr u32 size = sizeof(Quad);

	struct Texture;

	// 2d batch renderer
	struct BatchRenderer
	{
		u32 max_quads;
		u32 current_quads;

		PSO pso;
		TBuffer<Vertex2d> vertex_buffer;
		TBuffer<u16> index_buffer;

		Quad* quads;
		Vertex2d* vertices;
		const Texture* current_texture;

		Result create(u32 max_sprites,
			const char* vs=nullptr, const char* fs=nullptr
		);

		void clear();

		void begin(const Texture& texture);
		void end();

		void draw_sprite(glm::vec2 position, glm::vec4 src, float scale=1);

		void update_vertex_buffer();
	};
}

#endif