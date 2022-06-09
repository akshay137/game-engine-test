#include "uhero.h"
#include "core/logger.h"
#include "math/transforms.h"

#include <glad/glad.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_events.h>

#define MILLISECONDS_PER_SECOND 1000

result_t uhero_init(uhero_t* hero, int argc, char** args)
{
	assert(hero);
	assert(argc > 0);

	DUMPI(argc);
	DUMPCSTR(args[0]);

	result_t res = uhero_initDeps();
	if (UH_SUCCESS != res)
	{
		return res;
	}

	config_t config = config_read(UHERO_CONFIG_FILE);

	window_t window = {};
	gfx_t gfx = {};
	render2d_t r2d = {};
	renderstate_t rstate = {};
	buffer_t rsbuffer;

	res = window_new(&window, &config);
	if (UH_SUCCESS != res)
	{
		return res;
	}

	res = gfx_init(&gfx, &config, &window);
	if (UH_SUCCESS != res)
	{
		return res;
	}

	rstate.viewport = vec4(0, 0, window.width, window.height);
	rstate.orthographic = uhm_ortho(0, window.width, 0, window.height, -1, 1);
	rsbuffer = buffer_new(UHBUFFER_DYNAMIC, sizeof(rstate), &rstate);

	res = render2d_new(&r2d, config.max_sprites, window.width, window.height);
	if (UH_SUCCESS != res)
	{
		return res;
	}

	*hero = (uhero_t) {
		.config = config,
		.window = window,
		.gfx = gfx,
		.r2d = r2d,
		.rs_buffer = rsbuffer,
		.rstate = rstate,
		.delta = 0.16f,
		.time = 0.0f,

		.userdata = 0,
		._load = NULL,
		._clear = NULL,
		._update = NULL,
		._render = NULL,

		.exit_requested = false
	};

	return UH_SUCCESS;
}

void uhero_quit(uhero_t* hero)
{
	// clear current level
	if (hero->userdata)
	{
		hero->_clear(hero, hero->userdata);
	}

	render2d_delete(&(hero->r2d));
	buffer_delete(&(hero->rs_buffer));
	gfx_shutdown(&(hero->gfx));
	window_delete(&(hero->window));

	uhero_clearDeps();
}

result_t uhero_setLevel(uhero_t* hero, void* userdata,
	PFN_uheroLoad load, PFN_uheroClear clear,
	PFN_uheroUpdate update, PFN_uheroRender render
)
{
	assert(hero);

	// clear current level, if any
	if (hero->userdata)
	{
		hero->_clear(hero, hero->userdata);
	}

	hero->userdata = userdata;
	hero->_load = load;
	hero->_clear = clear;
	hero->_update = update;
	hero->_render = render;

	// load new level
	result_t res = hero->_load(hero, hero->userdata);
	return res;
}

float uhero_tick(uhero_t* hero)
{
	uint32_t ticks = SDL_GetTicks();
	uint32_t diff = ticks - hero->_ticks;
	hero->_ticks = ticks;
	float delta = diff / (float)MILLISECONDS_PER_SECOND;

	hero->delta = delta;
	hero->time = ticks / (float)MILLISECONDS_PER_SECOND;

	SDL_Event event = {};
	while (SDL_PollEvent(&event))
	{
		if (SDL_QUIT == event.type)
		{
			hero->exit_requested = true;
			return 0.0f;
		}
		else if (SDL_KEYUP == event.type)
		{
			if (SDLK_ESCAPE == event.key.keysym.sym)
			{
				hero->exit_requested = true;
				return 0.0;
			}
		}
	}

	hero->rstate.viewport = vec4(0, 0, hero->window.width, hero->window.height);
	hero->rstate.orthographic = uhm_ortho(
		0.0f, hero->window.width, 0.0f, hero->window.height,
		-1.0f, 1.0f
	);
	buffer_update(&(hero->rs_buffer), &(hero->rstate));
	buffer_bindBase(&(hero->rs_buffer), UHBUFFERBASE_UNIFORM, 0);

	// update
	if (hero->_update)
	{
		hero->_update(hero, hero->userdata);
	}

	// render
	glViewport(0, 0, hero->window.width, hero->window.height);

	float color[] = { 0, 0, 0, 0 };
	glClearBufferfv(GL_COLOR, 0, color);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

	if (hero->_render)
	{
		hero->_render(hero, hero->userdata);
	}

	window_swapBuffers(&(hero->window));

	return delta;
}
