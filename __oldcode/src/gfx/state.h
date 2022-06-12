#ifndef UHERO_RENDER_STATE_H__
#define UHERO_RENDER_STATE_H__ 1

#include "../core/core.h"
#include "../math/types.h"

struct renderstate_s
{
	mat4_t orthographic;
	vec4_t viewport;
};

#endif