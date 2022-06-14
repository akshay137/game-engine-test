#include "gfx.hpp"
#include "../logger.hpp"
#include "../window.hpp"

#include <glad/glad.h>
#include <SDL2/SDL_video.h>

namespace uhero::gfx
{
	void uh_gldebug_callback(GLenum source,
		GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message,
		const void *userParam
	);

	Result Context::create(Window& window, bool debug)
	{
		gl_context = SDL_GL_CreateContext(window.handle);
		if (nullptr == gl_context)
		{
			UHSDL_ERROR(SDL_GL_CreateContext);
			return Result::ExternalLibraryError;
		}

		if (0 == gladLoadGLLoader(SDL_GL_GetProcAddress))
		{
			UH_ERROR("Failed to OpenGL function pointers\n");
			return Result::ExternalLibraryError;
		}

		GLint val = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &val);
		api_version.major = val;
		glGetIntegerv(GL_MINOR_VERSION, &val);
		api_version.minor = val;

		if (debug)
		{
			UH_INFO("Enabling GL_DEBUG_OUTPUT\n");
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(uh_gldebug_callback, nullptr);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		UH_VERB("Created OpenGL context: %p\n", gl_context);

		return Result::Success;
	}

	void Context::clear()
	{
		assert(gl_context);

		UH_VERB("Destroying OpenGL context: %p\n", gl_context);

		SDL_GL_DeleteContext(gl_context);
		gl_context = nullptr;
	}

	void Context::clear_buffer(float* color, float depth, i32 stencil)
	{
		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
	}

	void uh_gldebug_callback(GLenum source,
		GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message,
		const void *userParam
	)
	{
		DUMPI(source);
		DUMPI(id);
		DUMPI(severity);
		DUMPI(length);
		DUMPPTR(userParam);

		if (GL_DEBUG_TYPE_ERROR == type)
		{
			UH_ERROR("%*s", length, message);
			assert(false);
		}
		else if (GL_DEBUG_TYPE_PERFORMANCE == type)
		{
			UH_WARN("%*s", length, message);
		}
		else
		{
			UH_INFO("%s", message);
		}
	}
}