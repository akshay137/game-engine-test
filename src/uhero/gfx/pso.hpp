#ifndef UHERO_GFX_PSO_H__
#define UHERO_GFX_PSO_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	struct Buffer;

	constexpr u32 MAX_VERTEX_ATTRIBUTES = 16;

	extern const u32 TRIANGLES;
	
	enum VertexAttribute : u16
	{
		Float = 0,
		Vec2,
		Vec3,
		Vec4,

		ByteNVec4,
		ShortNVec4,

		MaxVertexAttributes,
	};

	enum BlendState : u32
	{
		NoBlend = 0,
		
		OneMinusSrcAlpha,

		MaxBlendStates,
	};

	enum DepthState : u32
	{
		NoDepth = 0,

		Less,
		LessEqual,

		MaxDepthState,
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
		BlendState blend;
		DepthState depth;

		Result create(const VertexLayout& layout,
			const char* vertex_shader, const char* fragment_shader,
			BlendState blend=BlendState::NoBlend,
			DepthState depth=DepthState::NoDepth
		);
		void clear();

		void make_current();

		void set_vertex_buffer(const Buffer& vbuffer, u32 index,
			u32 offset, u32 stride
		);
		void set_index_buffer(const Buffer& ibuffer);

		void set_float(i32 index, f32 value);
		void set_int(i32 index, i32 value);
		void set_mat4(i32 index, f32* matrix);

		void set_blendstate(const BlendState state);
		void set_depthstate(const DepthState state);

		// draw commands
		void draw_elements(u32 primitive, u32 count, const void* offset);
		void draw_arrays(u32 primitive, u32 first, u32 count);

		// small optimization
		static inline u32 current_vao = 0;
		static inline u32 current_program = 0;
		static inline BlendState current_blend_state = BlendState::MaxBlendStates;
		static inline DepthState current_depth_state = DepthState::MaxDepthState;
	};
}

#endif