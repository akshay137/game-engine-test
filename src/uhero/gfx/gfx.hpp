#ifndef UHERO_GFX_H__
#define UHERO_GFX_H__ 1

#include "../common.hpp"
#include "../version.hpp"
#include "render_state.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "color.hpp"

namespace uhero
{
	struct Window;
}

namespace uhero::gfx
{
	struct GPURes
	{
		i32 created = 0;
		i32 deleted = 0;

		void create() { ++created; }
		void free() { ++deleted; }

		bool match() const { return created == deleted; }
	};

	struct GPUStats
	{
		u32 draw_calls;
		u32 triangle_count;
		u32 texture_switch;

		GPURes textures;
		GPURes buffers;
		GPURes shaders;

		void reset()
		{
			draw_calls = 0;
			triangle_count = 0;
			texture_switch = 0;
		}

		void dump_stats() const;
	};

	struct Context
	{
		void* gl_context;
		Version api_version;
		RenderState render_state;
		TBuffer<RenderState> rsbuffer;

		Result create(Window& window, bool debug = false);
		void clear();

		void set_vsync(bool);

		void clear_buffer(Color8 color, float depth=1.0f, i32 stencil=0);
		void update_render_state(float width, float height);

		void use_framebuffer(FrameBuffer& fbo);
		void use_default_framebuffer(const Window& window);

// debug
		static inline GPUStats gpu_stats;
		static void reset_stats() { gpu_stats.reset(); }

		// TODO: make this a normal way to create gl resources
		static void created_texture() { gpu_stats.textures.create(); }
		static void deleted_texture() { gpu_stats.textures.free(); }
		static void created_buffer() { gpu_stats.buffers.create(); }
		static void deleted_buffer() { gpu_stats.buffers.free(); }
		static void created_shader() { gpu_stats.shaders.create(); }
		static void deleted_shader() { gpu_stats.shaders.free(); }
	};
}

#endif