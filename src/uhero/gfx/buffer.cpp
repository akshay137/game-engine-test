#include "buffer.hpp"
#include "gfx.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result Buffer::create(BufferType type, u32 size, const void* data)
	{
		u32 flags = 0;
		if (BufferType::Dynaminc & type) flags |= GL_DYNAMIC_STORAGE_BIT;

		glCreateBuffers(1, &gl_id);
		glNamedBufferStorage(gl_id, size, data, flags);
		this->size = size;

		UH_VERB("Created buffer: %u[%ub]\n", gl_id, size);
		Context::created_buffer();

		return Result::Success;
	}

	void Buffer::clear()
	{
		UH_VERB("Deleting buffer: %u\n", gl_id);
		glDeleteBuffers(1, &gl_id);
		Context::deleted_buffer();
	}

	void Buffer::update_range(u32 offset, u32 length, const void* data)
	{
		assert(gl_id);

		glNamedBufferSubData(gl_id, offset, length, data);
	}

	void Buffer::bind_base_range(BufferBase base, u32 index, u32 offset, u32 size)
	{
		GLenum target;
		if (BufferBase::Uniform == base) target = GL_UNIFORM_BUFFER;
		else if (BufferBase::ShaderStorage == base) target = GL_SHADER_STORAGE_BUFFER;
		else assert(false);

		glBindBufferRange(target, index, gl_id, offset, size);
	}
}