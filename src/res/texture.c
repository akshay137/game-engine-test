#include "texture.h"
#include "../core/logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

result_t texture_loadFromFile(texture_t* out_tex, const char* file)
{
	assert(out_tex);
	assert(file);

	int32_t w = 0;
	int32_t h = 0;
	int32_t channels = 0;
	pixelformat_t format = UH_RGBA8;
	stbi_uc* pixels = stbi_load(file, &w, &h, &channels, 0);
	if (NULL == pixels)
	{
		ERROR("texture_loadFromFile: %s\n", file);
		return UH_IO_ERROR;
	}

	VERB("Loaded image: %s [%dx%d|%d]\n", file, w, h, channels);

	if (1 == channels) format = UH_GREYSCALE8;
	else if (3 == channels) format = UH_RGB8;
	else if (4 == channels) format = UH_RGBA8;
	
	result_t res = texture_new(out_tex, format, w, h, pixels);
	stbi_image_free(pixels);

	return res; 
}