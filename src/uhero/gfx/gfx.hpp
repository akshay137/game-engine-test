#ifndef UHERO_GFX_H__
#define UHERO_GFX_H__ 1

#include "../common.hpp"
#include "../version.hpp"

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

		Result create(Window& window, bool debug = false);
		void clear();

		void clear_buffer(float* color, float depth, i32 stencil);
	};
}

#endif