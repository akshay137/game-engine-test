#ifndef UHERO_WINDOW_H__
#define UHERO_WINDOW_H__ 1

#include "core.h"

// a wrapper around SDL_Window
// for some reasons
struct window_s
{
	struct SDL_Window* _window;
	int32_t width;
	int32_t height;
};

API result_t window_new(window_t* out_window, const config_t* config);
API void window_del(window_t* window);

API void window_swapBuffers(window_t* window);

#endif