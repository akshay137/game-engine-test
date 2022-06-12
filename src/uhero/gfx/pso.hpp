#ifndef UHERO_GFX_PSO_H__
#define UHERO_GFX_PSO_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	constexpr u32 MAX_VERTEX_ATTRIBUTES = 16;
	
	enum VertexAttribute : u16
	{
		Float = 1,
		Vec2,
		Vec3,
		Vec4,
	};

	struct VertexLayout
	{
		u32 attrib_count = 0;
		VertexAttribute attributes[MAX_VERTEX_ATTRIBUTES];

		u32 add_attribute(VertexAttribute attrib)
		{
			attributes[attrib_count++] = attrib;
			return attrib_count;
		}
	};

	// pipeline state object
	// contains vertex layout, OpenGL's vao,
	// shader program, and other states
	struct PSO
	{
		u32 vao;
		u32 program;

		VertexLayout layout;

		Result create(const VertexLayout& layout,
			const char* vertex_shader, const char* fragment_shader
		);
		void clear();

		void make_current();
	};
}

#endif