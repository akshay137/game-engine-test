#ifndef UHERO_H__
#define UHERO_H__ 1

#include "core/core.h"
#include "core/config.h"
#include "core/window.h"
#include "gfx/gfx.h"
#include "gfx/buffer.h"
#include "gfx/state.h"
#include "gfx/render2d.h"

typedef result_t (*PFN_uheroLoad)(uhero_t* hero, void* user);
typedef void (*PFN_uheroClear)(uhero_t* hero, void* user);
typedef void (*PFN_uheroUpdate)(uhero_t* hero, void* user);
typedef void (*PFN_uheroRender)(uhero_t* hero, void* user);

struct uhero_s
{
	config_t config;
	window_t window;
	gfx_t gfx;

	renderstate_t rstate;
	buffer_t rs_buffer;	
	render2d_t r2d;

	float delta;
	float time;
	uint32_t _ticks;

	// level data
	void* userdata;
	PFN_uheroLoad _load;
	PFN_uheroClear _clear;
	PFN_uheroUpdate _update;
	PFN_uheroRender _render;

	bool exit_requested;
};

API result_t uhero_init(uhero_t* hero, int argc, char** args);
API void uhero_quit(uhero_t* hero);

API result_t uhero_setLevel(uhero_t* hero, void* userdata,
	PFN_uheroLoad load, PFN_uheroClear clear,
	PFN_uheroUpdate update, PFN_uheroRender render
);

API float uhero_tick(uhero_t* hero);

API_INLINE bool uhero_exitRequested(const uhero_t* hero)
{
	assert(hero);

	return hero->exit_requested;
}
#endif