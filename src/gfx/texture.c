#include "texture.h"
#include "../core/logger.h"

#include <glad/glad.h>

result_t texture_new(texture_t* out_tex, pixelformat_t format,
	int32_t width, int32_t height, const void* pixels
)
{
	GLenum internal_format = 0;
	GLenum channel_format = 0;
	GLenum data_type = 0;
	GLint swizzel[4] = { 0 };

	if (UH_GREYSCALE8 == format)
	{
		internal_format = GL_R8;
		channel_format = GL_RED;
		data_type = GL_UNSIGNED_BYTE;
		swizzel[0] = GL_RED;
		swizzel[1] = GL_RED;
		swizzel[2] = GL_RED;
		swizzel[3] = GL_ONE;
	}
	else if (UH_RGB8 == format)
	{
		internal_format = GL_RGB8;
		channel_format = GL_RGB;
		data_type = GL_UNSIGNED_BYTE;
		swizzel[0] = GL_RED;
		swizzel[1] = GL_GREEN;
		swizzel[2] = GL_BLUE;
		swizzel[3] = GL_ONE;
	}
	else if (UH_RGBA8 == format)
	{
		internal_format = GL_RGBA8;
		channel_format = GL_RGB;
		data_type = GL_UNSIGNED_BYTE;
		swizzel[0] = GL_RED;
		swizzel[1] = GL_GREEN;
		swizzel[2] = GL_BLUE;
		swizzel[3] = GL_ALPHA;
	}

	uint32_t id = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, internal_format, width, height);
	glTextureSubImage2D(id, 0,
		0, 0, width, height,
		channel_format, data_type, pixels
	);
	glTextureParameteriv(id, GL_TEXTURE_SWIZZLE_RGBA, swizzel);

	*out_tex = (texture_t) {
		.gl_id = id,
		.width = width,
		.height = height,
		.pixel_format = format
	};

	VERB("Texture created: %u [%dx%d][%x]\n", id, width, height, format);

	return UH_SUCCESS;
}

void texture_delete(texture_t* tex)
{
	assert(tex);

	VERB("Deleting texture: %u\n", tex->gl_id);
	glDeleteTextures(1, &(tex->gl_id));
	tex->gl_id = 0;
}

void texture_setFiltering(texture_t* tex, texfilter_t filter)
{
	assert(tex);

	uint32_t min_filter = 0;
	uint32_t mag_filter = 1;

	if (UH_FILTER_LINEAR == filter)
	{
		min_filter = GL_LINEAR;
		mag_filter = GL_LINEAR;
	}
	else if (UH_FILTER_NEAREST == filter)
	{
		min_filter = GL_NEAREST;
		mag_filter = GL_NEAREST;
	}

	glTextureParameteri(tex->gl_id, GL_TEXTURE_MIN_FILTER, min_filter);
	glTextureParameteri(tex->gl_id, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void texture_bindSlost(texture_t* tex, uint32_t slot)
{
	assert(tex);

	glBindTextureUnit(slot, tex->gl_id);
}