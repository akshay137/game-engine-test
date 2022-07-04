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
		UH_INFO("OpenGL Info:\n"
			"\tversion: %s\n"
			"\tvendor: %s\n"
			"\trenderer: %s\n",
			glGetString(GL_VERSION),
			glGetString(GL_VENDOR),
			glGetString(GL_RENDERER)
		);
		
		render_state.resize(window.width, window.height);
		rsbuffer.create(BufferType::Dynaminc, 1, &render_state);

		reset_stats();

		return Result::Success;
	}

	void Context::clear()
	{
		assert(gl_context);

		UH_VERB("Destroying OpenGL context: %p\n", gl_context);

		rsbuffer.clear();
		SDL_GL_DeleteContext(gl_context);
		gl_context = nullptr;
	}

	void Context::set_vsync(bool status)
	{
		SDL_GL_SetSwapInterval(status ? 1 : 0);
	}

	void Context::clear_buffer(Color32 color, float depth, i32 stencil)
	{
		float cc[4];
		color.to_rgba(cc[0], cc[1], cc[2], cc[3]);
		auto vp = render_state.viewport;
		glViewport(vp.x, vp.y, vp.z, vp.w);
		glClearBufferfv(GL_COLOR, 0, cc);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
	}

	void Context::update_render_state(float width, float height)
	{
		render_state.resize(width, height);
		rsbuffer.update(&render_state);
		rsbuffer.bind_base(BufferBase::Uniform, 0);
		glViewport(0, 0, width, height);
	}

	void Context::use_framebuffer(FrameBuffer& fbo)
	{
		fbo.make_current();
		update_render_state(fbo.width, fbo.height);
	}

	void Context::use_default_framebuffer(const Window& window)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		update_render_state(window.width, window.height);
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
			UH_ERROR("%s\n", message);
			assert(false);
		}
		else if (GL_DEBUG_TYPE_PERFORMANCE == type)
		{
			UH_WARN("%s\n", message);
		}
		else
		{
			UH_INFO("%s\n", message);
		}
	}

	void GPUStats::dump_stats() const
	{
		UH_INFO("Textures: {created: %d| deleted: %d}\n", textures.created, textures.deleted);
		UH_INFO("Buffers: {created: %d| deleted: %d}\n", buffers.created, buffers.deleted);
		UH_INFO("Shaders: {created: %d| deleted: %d}\n", shaders.created, shaders.deleted);
	}
}