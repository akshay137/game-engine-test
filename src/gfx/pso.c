#include "pso.h"
#include "program.h"
#include "buffer.h"
#include "../ds/str.h"
#include "../core/logger.h"

#include <glad/glad.h>

result_t pso_new(pso_t* out_pso)
{
	uint32_t vao = 0;
	uint32_t program = 0;
	glCreateVertexArrays(1, &vao);
	program = glCreateProgram();

	*out_pso = (pso_t) {
		.vao = vao,
		.program = program,
	};

	VERB("PSO [vao: %u | sp: %u]\n",
		out_pso->vao, out_pso->program
	);

	return UH_SUCCESS;
}

void pso_delete(pso_t* pso)
{
	assert(pso);

	VERB("Deleting PSO [vao: %u | sp: %u]\n",
		pso->vao, pso->program
	);
	glDeleteVertexArrays(1, &(pso->vao));
	glDeleteProgram(pso->program);
}

result_t pso_create(pso_t* out_pso,
	str_t* shaders,
	vattrib_t* vertex_attribs,
	geom_t geom_type, fillmode_t fillmode, cullmode_t cullmode
)
{
	assert(out_pso);
	assert(shaders);
	assert(vertex_attribs);

	pso_t pso = {
		._geometry = geom_type,
		._fill = fillmode,
		._cull = cullmode,
	};

	uint32_t index = 0;

	glCreateVertexArrays(1, &(pso.vao));
	uint32_t relative_offset = 0;
	while (UHVATTRIB_NONE != vertex_attribs[index])
	{
		const vattrib_t attrib = vertex_attribs[index];
		
		uint32_t element_count = 0;
		uint32_t gl_type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;
		uint32_t byte_size = 0;
		uint32_t buffer_binding = 0;

		if (UHVATTRIB_VEC2 == attrib)
		{
			element_count = 2;
			gl_type = GL_FLOAT;
			normalized = GL_FALSE;
			byte_size = sizeof(float) * 2;
		}
		else if (UHVATTRIB_VEC3 == attrib)
		{
			element_count = 3;
			gl_type = GL_FLOAT;
			normalized = GL_FALSE;
			byte_size = sizeof(float) * 3;
		}
		else if (UHVATTRIB_VEC4 == attrib)
		{
			element_count = 4;
			gl_type = GL_FLOAT;
			normalized = GL_FALSE;
			byte_size = sizeof(float) * 4;
		}
		else if (UHVATTRIB_BVEC4_NORM == attrib)
		{
			element_count = 4;
			gl_type = GL_UNSIGNED_BYTE;
			normalized = GL_TRUE;
			byte_size = 4;
		}

		glVertexArrayAttribFormat(pso.vao, index,
			element_count, gl_type, normalized, relative_offset
		);
		glVertexArrayAttribBinding(pso.vao, index, buffer_binding);
		glEnableVertexArrayAttrib(pso.vao, index);

		relative_offset += byte_size;
		++index;
	}

	pso.program = uh_createShaderProgramVS(shaders[0], shaders[1]);

	*out_pso = pso;
	return UH_SUCCESS;
}

void pso_makeCurrent(pso_t* pso)
{
	assert(pso);

	glBindVertexArray(pso->vao);
	glUseProgram(pso->program);
}

void pso_setVertexBuffer(pso_t* pso, uint32_t index,
	buffer_t* buffer, uint32_t offset, uint32_t stride
)
{
	assert(pso);
	assert(buffer);

	glVertexArrayVertexBuffer(pso->vao, index, buffer->gl_id, offset, stride);
}

void pso_setIndexBuffer(pso_t* pso, buffer_t* buffer)
{
	assert(pso);
	assert(buffer);

	glVertexArrayElementBuffer(pso->vao, buffer->gl_id);
}

void pso_drawArrays(pso_t* pso, uint32_t first, uint32_t count)
{
	assert(pso);

	uint32_t geometry = GL_TRIANGLES;
	glDrawArrays(geometry, first, count);
}