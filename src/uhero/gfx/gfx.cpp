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

		return Result::Success;
	}

	void Context::clear()
	{
		assert(gl_context);

		SDL_GL_DeleteContext(gl_context);
		gl_context = nullptr;
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
			UH_ERROR("%s", message);
			assert(false);
		}
		else if (GL_DEBUG_TYPE_PERFORMANCE == type)
		{
			UH_WARN("%s", message);
		}
		else
		{
			UH_INFO("%s", message);
		}
	}
}