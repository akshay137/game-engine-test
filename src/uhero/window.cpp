#include "window.hpp"
#include "logger.hpp"

#include <SDL2/SDL_video.h>

namespace uhero
{
	Result Window::create_window(const char* title, i32 w, i32 h,
		i32 display, u32 flags
	)
	{
		auto pos = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
		uint32_t sdl_flags = SDL_WINDOW_OPENGL;
		if (WindowFlags::FullScreen & flags)
		{
			sdl_flags |= SDL_WINDOW_FULLSCREEN;
		}

		handle = SDL_CreateWindow(title,
			pos, pos, w, h,
			sdl_flags
		);
		if (nullptr == handle)
		{
			UHSDL_ERROR(SDL_CreateWindow);
			return Result::ExternalLibraryError;
		}

		this->title = title;
		this->width = w;
		this->height = h;

		UH_VERB("Window[%s] created <%dx%d> @%p\n", title, w, h, handle);
		return Result::Success;
	}

	void Window::close()
	{
		UH_VERB("Destroying window[%s] from %p\n", title, handle);
		
		SDL_DestroyWindow(handle);
		handle = nullptr;
	}

	void Window::swap_buffers()
	{
		assert(handle);

		SDL_GL_SwapWindow(handle);
	}

	Result Window::setup_opengl_properties()
	{
		i32 res = 0;
		res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE
		);

		res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
			OPENGL_MAJOR_VERSION
		);
		res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
			OPENGL_MINOR_VERSION
		);

		return Result::Success;
	}
}