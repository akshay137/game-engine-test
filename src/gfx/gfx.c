#include "gfx.h"
#include "../core/config.h"
#include "../core/window.h"
#include "../core/logger.h"

#include <glad/glad.h>
#include <SDL2/SDL_video.h>

void uhglDebugCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message,
	const void *userParam
);

result_t gfx_init(gfx_t* out_gfx, const config_t* conf, const window_t* window)
{
	assert(out_gfx);
	assert(conf);
	assert(window);

	SDL_GLContext ctx = SDL_GL_CreateContext(window->_window);
	if (NULL == ctx)
	{
		ERROR("SDL_GL_CreateContext: %s\n", SDL_GetError());
		return UH_EXTERNERL_LIB_ERROR;
	}

	if (0 == gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		ERROR("gladLoadGLLoader\n");
		return UH_EXTERNERL_LIB_ERROR;
	}


	// enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(uhglDebugCallback, NULL);

	GLint major = 0;
	GLint minor = 0;
	GLint patch = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	str_t gpu = str((const char*)glGetString(GL_RENDERER));

	*out_gfx = (gfx_t) {
		._context = ctx,
		.api_version = version(major, minor, patch),
		.gpu_name = gpu
	};

	DUMPVER(out_gfx->api_version);
	DUMPSTR(out_gfx->gpu_name);


	return UH_SUCCESS;
}

void gfx_shutdown(gfx_t* gfx)
{
	assert(gfx);
	assert(gfx->_context);

	VERB("Deleting GLContext: %p\n", gfx->_context);
	SDL_GL_DeleteContext(gfx->_context);
}

void uhglDebugCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message,
	const void *userParam
)
{
	DUMPX(severity);
	DUMPX(source);
	DUMPU(id);
	DUMPU(length);
	DUMPPTR(userParam);

	if (type == GL_DEBUG_TYPE_ERROR)
	{
		ERROR("%s\n", message);
	}
	else if (type == GL_DEBUG_TYPE_PERFORMANCE)
	{
		WARN("%s\n", message);
	}
	else
	{
		VERB("%s\n", message);
	}
}