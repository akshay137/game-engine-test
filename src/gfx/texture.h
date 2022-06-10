#ifndef UHERO_TEXTURE_H__
#define UHERO_TEXTURE_H__ 1

#include "../core/core.h"

enum pixelformat_e
{
	UH_GREYSCALE8 = 0,
	UH_RGB8 = 1,
	UH_RGBA8 = 2,
};
typedef enum pixelformat_e pixelformat_t;

enum texfilter_e
{
	UH_FILTER_LINEAR = 0,
	UH_FILTER_NEAREST = 1,
};
typedef enum texfilter_e texfilter_t;

struct texture_s
{
	uint32_t gl_id;
	int32_t width;
	int32_t height;
	pixelformat_t pixel_format;
};

API result_t texture_new(texture_t* out_tex, pixelformat_t format,
	int32_t width, int32_t height, const void* pixels
);
API void texture_delete(texture_t* tex);

API void texture_setFiltering(texture_t* tex, texfilter_t filter);
API void texture_bindSlot(texture_t* tex, uint32_t slot);

#endif