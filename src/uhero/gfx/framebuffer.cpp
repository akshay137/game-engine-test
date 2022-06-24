#include "framebuffer.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result FrameBuffer::create(FBDescriptor descriptor, i32 w, i32 h)
	{
		glCreateFramebuffers(1, &gl_id);

		for (u32 i = 0; i < descriptor.color_attachments; i++)
		{
			color[i].create(w, h, descriptor.attachments[i]);
			color[i].set_filter(TextureFilter::Nearest);

			glNamedFramebufferTexture(gl_id,
				GL_COLOR_ATTACHMENT0 + i,
				color[i].gl_id, 0
			);
			glNamedFramebufferDrawBuffer(gl_id, GL_COLOR_ATTACHMENT0 + i);
		}

		depth.create(w, h, PixelFormat::Depth24Stencil8);
		depth.set_filter(TextureFilter::Nearest);
		glNamedFramebufferTexture(gl_id, GL_DEPTH_STENCIL_ATTACHMENT, depth.gl_id, 0);

		this->descriptor = descriptor;
		this->width = w;
		this->height = h;

		if (!is_ready())
		{
			UH_ERROR("FBO: %u is not ready\n", gl_id);
			this->clear();
			return Result::ExternalLibraryError;
		}

		UH_VERB("Created FBO: %u [%u]\n", gl_id, descriptor.color_attachments);

		return Result::Success;
	}

	void FrameBuffer::clear()
	{
		assert(gl_id);

		UH_VERB("Deleting FBO: %u\n", gl_id);
		depth.clear();
		for (u32 i = 0; i < descriptor.color_attachments; i++)
			color[i].clear();
		
		glDeleteFramebuffers(1, &gl_id);
		gl_id = 0;
	}

	bool FrameBuffer::is_ready() const
	{
		GLenum stat = glCheckNamedFramebufferStatus(gl_id, GL_FRAMEBUFFER);
		bool res = GL_FRAMEBUFFER_COMPLETE == stat;
		return res;
	}

	void FrameBuffer::make_current()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gl_id);

		// clear buffers
		float clear_color[4] = { 0, 0, 0, 0 };
		clear_buffers(clear_color, 1.0f, 0);
	}

	void FrameBuffer::clear_buffers(const float* clear_color, float depth, i32 stencil)
	{
		for (u32 i = 0; i < descriptor.color_attachments; i++)
		{
			glClearNamedFramebufferfv(gl_id, GL_COLOR, i, clear_color);
		}
		glClearNamedFramebufferfi(gl_id, GL_DEPTH_STENCIL, 0, depth, stencil);
	}
}