#ifndef UHERO_WINDOW_H__
#define UHERO_WINDOW_H__ 1

#include "common.hpp"

struct SDL_Window;

namespace uhero
{
	struct Config;

	enum WindowFlags : u32
	{
		Default = 0,
		FullScreen = 1 << 0,
		Borderless = 1 << 1,
	};

	struct Window
	{
		SDL_Window* handle;
		const char* title;
		i32 width;
		i32 height;

		Result create_window(const char* title, i32 w, i32 h,
			i32 display = 0, u32 flags = 0
		);
		void close();

		static Result setup_opengl_properties();
	};
}

#endif