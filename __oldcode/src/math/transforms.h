#ifndef UHERO_MATH_TRANSFORMS_H__
#define UHERO_MATH_TRANSFORMS_H__ 1

#include "types.h"

API mat4_t uhm_ortho(float left, float right, float bottom, float top,
	float near, float far
);

#endif