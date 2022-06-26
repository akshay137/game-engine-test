#include "utils.hpp"
#include "pso.hpp"
#include "gfx.hpp"
#include "../memory/memory.hpp"
#include "../file.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	struct VertexAttribData
	{
		u32 count;
		u32 gl_type;
		GLboolean normalized;
		u32 byte_size;

		VertexAttribData() = default;

		constexpr VertexAttribData(u32 count, u32 gl_type, bool normalized, u32 byte_size)
			: count{count}, gl_type{gl_type}, normalized{normalized},
				byte_size{byte_size}
		{}
	};

	constexpr auto max_vdatas = static_cast<u32>(VertexAttribute::MaxVertexAttributes);
	constexpr VertexAttribData vdata[max_vdatas] = {
		[ENUM_INT(VertexAttribute::Float)] = VertexAttribData(1, GL_FLOAT, GL_FALSE, sizeof(float)),
		[ENUM_INT(VertexAttribute::Vec2)] = VertexAttribData(2, GL_FLOAT, GL_FALSE, sizeof(float) * 2),
		[ENUM_INT(VertexAttribute::Vec3)] = VertexAttribData(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3),
		[ENUM_INT(VertexAttribute::Vec4)] = VertexAttribData(4, GL_FLOAT, GL_FALSE, sizeof(float) * 4),
		[ENUM_INT(VertexAttribute::ByteNVec4)] = VertexAttribData(4, GL_UNSIGNED_BYTE, GL_TRUE, 4)
	};

	bool vertex_data_from_attribute(VertexAttribute attrib, VertexAttribData& out)
	{
		u32 index = static_cast<u32>(attrib);
		if (index > max_vdatas) return false;

		out = vdata[index];
		return true;
	}

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
			VertexAttribData data;
			if (!vertex_data_from_attribute(layout.attributes[i], data))
			{
				UH_ERROR("Invalid VertexAttribute: %u\n", layout.attributes[i]);
				assert(false);
			}

			glVertexArrayAttribFormat(vao, i,
				data.count, data.gl_type, data.normalized, relative_offset
			);
			glVertexArrayAttribBinding(vao, i, 0);
			glEnableVertexArrayAttrib(vao, i);

			relative_offset += data.byte_size;
		}

		return vao;
	}

	u32 Utils::load_shader_from_file(const char* file, u32 gl_shader_type)
	{
		UH_INFO("Loading shader file: %s\n", file);

		u32 shader = glCreateShader(gl_shader_type);
		if (0 == shader)
		{
			return Result::ExternalLibraryError;
		}

		isize source_size = 0;
		char* source = (char*)File::read_full_framestack(file, source_size);

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

		Context::created_shader();
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
		Context::deleted_shader();
		glDeleteShader(fshader);
		Context::deleted_shader();

		GLint status = GL_TRUE;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (GL_TRUE != status)
		{
			UH_ERROR("Program Link Error\n");
			glDeleteProgram(program);
			return Result::ExternalLibraryError;
		}

		Context::created_shader();
		return program;
	}
}