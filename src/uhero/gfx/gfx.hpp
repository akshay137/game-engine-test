#ifndef UHERO_GFX_H__
#define UHERO_GFX_H__ 1

#include "../common.hpp"
#include "../version.hpp"
#include "render_state.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"

namespace uhero
{
	struct Window;
}

namespace uhero::gfx
{
	struct GPUStats
	{
		u32 draw_calls;
		u32 triangle_count;
		u32 texture_switch;

		void reset()
		{
			draw_calls = 0;
			triangle_count = 0;
			texture_switch = 0;
		}
	};

	struct Context
	{
		void* gl_context;
		Version api_version;
		RenderState render_state;
		TBuffer<RenderState> rsbuffer;

		Result create(Window& window, bool debug = false);
		void clear();

		void clear_buffer(float* color, float depth, i32 stencil);
		void update_render_state(float width, float height);

		void use_framebuffer(FrameBuffer& fbo);
		void use_default_framebuffer();

		static inline GPUStats gpu_stats;
		static void reset_stats() { gpu_stats.reset(); }
	};
}

#endif