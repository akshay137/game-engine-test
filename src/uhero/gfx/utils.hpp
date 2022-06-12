#ifndef UHERO_GFX_UTILS_H__
#define UHERO_GFX_UTILS_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	struct VertexLayout;

	// empty struct, only here for helper functions
	struct Utils
	{
		static u32 create_vao_from_layout(const VertexLayout& layout);

		static u32 load_shader_from_file(const char* file, u32 gl_shader_type);
		static u32 create_program_from_vf(const char* vs, const char* fs);
	};
}

#endif