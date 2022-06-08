#ifndef UHERO_GPU_BUFFER_H__
#define UHERO_GPU_BUFFER_H__ 1

#include "../core/core.h"

// flags
enum buffertype_e
{
	UHBUFFER_STATIC = 0,
	UHBUFFER_DYNAMIC = 1,
};
typedef enum buffertype_e buffertype_t;

enum bufferbase_e
{
	UHBUFFERBASE_UNIFORM = 0,
	UHBUFFERBASE_SHADER_STORAGE = 1
};
typedef enum bufferbase_e bufferbase_t;

struct buffer_s
{
	uint32_t gl_id;
	uint32_t size;
	buffertype_t type;
};

API buffer_t buffer_new(buffertype_t type, uint32_t size, const void* data);
API void buffer_delete(buffer_t* buffer);

API void buffer_bindBase(buffer_t* buffer, bufferbase_t base, uint32_t index);

#endif