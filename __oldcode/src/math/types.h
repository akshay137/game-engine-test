#ifndef UHERO_MATH_TYPES_H__
#define UHERO_MATH_TYPES_H__ 1

#include "../core/core.h"

UH_TDEF_UNION(vec2);
UH_TDEF_UNION(ivec2);
UH_TDEF_UNION(vec3);
UH_TDEF_UNION(vec4);

UH_TDEF_UNION(mat2);
UH_TDEF_UNION(mat3);
UH_TDEF_UNION(mat4);

union vec2_u
{
	float arr[2];
	struct { float x, y; };
};

union ivec2_u
{
	int32_t arr[2];
	struct { int32_t x, y; };
};

union vec3_u
{
	float arr[3];
	struct { float x, y, z; };
};

union vec4_u
{
	float arr[4];
	struct { float x, y, z, w; };
};

union mat2_u
{
	float arr[4];
	struct { vec2_t c0, c1; };
	struct {
		float m00, m10; // column 0
		float m01, m11; // column 1
	};
};

union mat3_u
{
	float arr[9];
	struct { vec3_t c0, c1, c2; };
	struct {
		float m00, m10, m20; // column 0
		float m01, m11, m21; // column 1
		float m02, m12, m22; // column 2
	};
};

union mat4_u
{
	float arr[16];
	struct { vec4_t c0, c1, c2, c3; };
	struct {
		float m00, m10, m20, m30; // column 0
		float m01, m11, m21, m31; // column 1
		float m02, m12, m22, m32; // column 2
		float m03, m13, m23, m33; // column 3
	};
};


// helper functions for constructing math types

API_INLINE vec2_t vec2(float x, float y)
{
	vec2_t res = { .x = x, .y = y };
	return res;
}

API_INLINE ivec2_t ivec2(int32_t x, int32_t y)
{
	ivec2_t res = { .x = x, .y = y };
	return res;
}

API_INLINE vec3_t vec3(float x, float y, float z)
{
	vec3_t res = { .x = x, .y = y, .z = z };
	return res;
}

API_INLINE vec4_t vec4(float x, float y, float z, float w)
{
	vec4_t res = { .x = x, .y = y, .z = z, .w = w };
	return res;
}

API_INLINE mat4_t mat4(float v)
{
	mat4_t m = {};
	m.m00 = v;
	m.m11 = v;
	m.m22 = v;
	m.m33 = v;
	return m;
};

API_INLINE mat4_t mat4_identity(void)
{
	return mat4(1.0f);
}

#endif