#ifndef UHERO_PROGRAM_H__
#define UHERO_PROGRAM_H__ 1

#include "../core/core.h"
#include "../ds/str.h"

enum shader_e
{
	VERTEX = 0,
	FRAGMENT = 1
};
typedef enum shader_e shader_t;

API uint32_t uh_createShaderProgramVS(str_t vs, str_t fs);

API uint32_t uh_createShader(str_t s, shader_t type);

#endif