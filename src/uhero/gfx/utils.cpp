#include "utils.hpp"
#include "pso.hpp"
#include "../file.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	u32 Utils::create_vao_from_layout(const VertexLayout& layout)
	{
		u32 vao = 0;
		glCreateVertexArrays(1, &vao);
		if (0 == vao)
		{
			return Result::ExternalLibraryError;
		}

		u32 relative_offset = 0;
		for (usize i = 0; i < layout.attrib_count; i++)
		{
			u32 count = 0;
			u32 gl_type = GL_FLOAT;
			u32 offset = relative_offset;
			GLboolean normalized = GL_FALSE;

			switch (layout.attributes[i])
			{
				case VertexAttribute::Float:
				{
					count = 1;
					gl_type = GL_FLOAT;
					offset = relative_offset;
					normalized = GL_FALSE;
					relative_offset += sizeof(float);
				}
					break;
				
				case VertexAttribute::Vec2:
				{
					count = 2;
					gl_type = GL_FLOAT;
					offset = relative_offset;
					normalized = GL_FALSE;
					relative_offset += (sizeof(float) * 2);
				}
					break;
				
				case VertexAttribute::Vec3:
				{
					count = 3;
					gl_type = GL_FLOAT;
					offset = relative_offset;
					normalized = GL_FALSE;
					relative_offset += (sizeof(float) * 3);
				}
					break;
				
				case VertexAttribute::Vec4:
				{
					count = 4;
					gl_type = GL_FLOAT;
					offset = relative_offset;
					normalized = GL_FALSE;
					relative_offset += (sizeof(float) * 2);
				}
					break;
				
				default:
				{
					UH_ERROR("Invalid VertexAttribute: %u\n",
						layout.attributes[i]);
				}
					break;
			}

			glVertexArrayAttribFormat(vao, i, count, gl_type, normalized, offset);
			glVertexArrayAttribBinding(vao, i, 0);
			glEnableVertexArrayAttrib(vao, i);
		}

		return vao;
	}

	u32 Utils::load_shader_from_file(const char* file, u32 gl_shader_type)
	{
		u32 shader = glCreateShader(gl_shader_type);
		if (0 == shader)
		{
			return Result::ExternalLibraryError;
		}

		char source[2048];
		usize bread = File::read_full(file, source, 2048);
		source[bread] = 0;

		const GLchar* sources[] = { source };
		glShaderSource(shader, 1, sources, nullptr);
		glCompileShader(shader);

		GLint status = GL_TRUE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (GL_TRUE != status)
		{
			UH_ERROR("Shader compile error: %s\n", file);
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	u32 Utils::create_program_from_vf(const char* vs, const char* fs)
	{
		u32 vshader = load_shader_from_file(vs, GL_VERTEX_SHADER);
		if (0 == vshader)
		{
			return 0;
		}

		u32 fshader = load_shader_from_file(fs, GL_FRAGMENT_SHADER);
		if (0 == fshader)
		{
			glDeleteShader(vshader);
			return 0;
		}

		u32 program = glCreateProgram();
		if (0 == program)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			return Result::ExternalLibraryError;
		}

		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);
		glDetachShader(program, vshader);
		glDetachShader(program, fshader);
		glDeleteShader(vshader);
		glDeleteShader(fshader);

		GLint status = GL_TRUE;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (GL_TRUE != status)
		{
			UH_ERROR("Program Link Error\n");
			glDeleteProgram(program);
			return Result::ExternalLibraryError;
		}

		return program;
	}
}