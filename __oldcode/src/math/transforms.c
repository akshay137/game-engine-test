#include "transforms.h"

mat4_t uhm_ortho(float left, float right, float bottom, float top,
	float near, float far
)
{
	mat4_t ortho = mat4(1.0f);

	ortho.m00 = 2.0f / (right - left);
	ortho.m11 = 2.0f / (top - bottom);
	ortho.m22 = -2.0f / (far - near);

	ortho.m03 = -(right + left) / (right - left);
	ortho.m13 = -(top + bottom) / (top - bottom);
	ortho.m23 = -(far + near) / (far - near);

	return ortho;
}