#ifndef UHERO_GFX_H__
#define UHERO_GFX_H__ 1

#include "../common.hpp"
#include "../version.hpp"
#include "render_state.hpp"
#include "buffer.hpp"

namespace uhero
{
	struct Window;
}

namespace uhero::gfx
{
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
	};
}

#endif