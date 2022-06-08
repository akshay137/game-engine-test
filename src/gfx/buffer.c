#include "buffer.h"
#include "../core/logger.h"

#include <glad/glad.h>

buffer_t buffer_new(buffertype_t type, uint32_t size, const void* data)
{
	uint32_t buffer_id = 0;
	glCreateBuffers(1, &buffer_id);

	uint32_t flags = 0;
	if (UHBUFFER_DYNAMIC & flags) flags |= GL_DYNAMIC_STORAGE_BIT;
	glNamedBufferStorage(buffer_id, size, data, flags);

	buffer_t buffer = {
		.gl_id = buffer_id,
		.size = size,
		.type = type
	};

	VERB("Created OpenGL buffer: %u [%u|%x]\n", buffer_id, size, type);
	return buffer;
}

void buffer_delete(buffer_t* buffer)
{
	assert(buffer);
	assert(buffer->gl_id);

	VERB("Deleting OpenGL bufer: %u\n", buffer->gl_id);
	glDeleteBuffers(1, &(buffer->gl_id));
	buffer->gl_id = 0;
}

void buffer_bindBase(buffer_t* buffer, bufferbase_t base, uint32_t index)
{
	assert(buffer);

	uint32_t target = 0;
	if (UHBUFFERBASE_UNIFORM == base) target = GL_UNIFORM_BUFFER;
	if (UHBUFFERBASE_SHADER_STORAGE == base) target = GL_SHADER_STORAGE_BUFFER;
	glBindBufferBase(target, index, buffer->gl_id);
}