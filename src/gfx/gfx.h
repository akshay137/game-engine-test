#ifndef UHERO_GFX_H__
#define UHERO_GFX_H__ 1

#include "../core/core.h"
#include "../core/version.h"
#include "../ds/str.h"

struct gfx_s
{
	void* _context; // SDL_GLContext
	version_t api_version;
	str_t gpu_name;
};

API result_t gfx_init(gfx_t* out_gfx, const config_t* conf, const window_t* window);
API void gfx_shutdown(gfx_t* gfx);


#endif