#ifndef UHERO_VERTEX_H__
#define UHERO_VERTEX_H__ 1

#include "../core/core.h"
#include "../math/types.h"

struct vertex2d_s
{
	vec2_t pos;
	vec2_t uv;
	vec3_t color;
};

struct vertex3d_s
{
	vec3_t pos;
	vec2_t uv;
	vec3_t normal;
};

API_INLINE vertex2d_t vertex2d(vec2_t pos, vec2_t uv, vec3_t color)
{
	vertex2d_t v = {
		.pos = pos,
		.uv = uv,
		.color = color
	};
	return v;
};

API_INLINE vertex3d_t vertex3d(vec3_t pos, vec2_t uv, vec3_t normal)
{
	vertex3d_t v = {
		.pos = pos,
		.uv = uv,
		.normal = normal
	};
	return v;
}

#endif