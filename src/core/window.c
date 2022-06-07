#include "window.h"
#include "config.h"
#include "logger.h"

#include <SDL2/SDL_video.h>

result_t window_new(window_t* out_window, const config_t* config)
{
	assert(config);

	// set opengl flags
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	uint32_t flags = SDL_WINDOW_OPENGL;
	int32_t pos = SDL_WINDOWPOS_UNDEFINED_DISPLAY(config->display_index);
	SDL_Window* window = SDL_CreateWindow(config->app_name.ptr,
		pos, pos, config->width, config->height,
		flags
	);
	if (NULL == window)
	{
		ERROR("SDL_CreateWindow : %s\n", SDL_GetError());
		return UH_EXTERNERL_LIB_ERROR;
	}

	*out_window = (window_t) {
		._window = window,
		.width = config->width,
		.height = config->height
	};
	DUMPPTR(out_window->_window);
	return UH_SUCCESS;
}

void window_del(window_t* window)
{
	assert(window);
	assert(window->_window);

	SDL_DestroyWindow(window->_window);
	window->_window = NULL;
}

void window_swapBuffers(window_t* window)
{
	assert(window);
	assert(window->_window);
	
	SDL_GL_SwapWindow(window->_window);
}