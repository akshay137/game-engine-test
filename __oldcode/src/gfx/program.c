#include "program.h"
#include "../core/logger.h"

#include <glad/glad.h>

uint32_t uh_createShaderProgramVS(str_t vs, str_t fs)
{
	uint32_t vss = uh_createShader(vs, VERTEX);
	uint32_t fss = uh_createShader(fs, FRAGMENT);

	uint32_t program = glCreateProgram();
	glAttachShader(program, vss);
	glAttachShader(program, fss);
	glLinkProgram(program);

	glDetachShader(program, vss);
	glDetachShader(program, fss);
	glDeleteShader(vss);
	glDeleteShader(fss);

	GLint stat = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &stat);
	if (GL_TRUE != stat)
	{
		ERROR("Failed to create program\n");
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

uint32_t uh_createShader(str_t s, shader_t type)
{
	uint32_t stype = GL_VERTEX_SHADER;
	if (VERTEX == type) stype = GL_VERTEX_SHADER;
	if (FRAGMENT == type) stype = GL_FRAGMENT_SHADER;

	uint32_t shader = glCreateShader(stype);

	const GLchar* source[] = { s.ptr };
	glShaderSource(shader, 1, source, NULL);
	glCompileShader(shader);

	GLint stat = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
	if (GL_TRUE != stat)
	{
		ERROR("Failed to compile shader\n");
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}
