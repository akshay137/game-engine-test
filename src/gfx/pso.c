#include "pso.h"
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
		.framebuffer = 0
	};

	VERB("PSO [vao: %u | sp: %u | fb: %u]\n",
		out_pso->vao, out_pso->program, out_pso->framebuffer
	);

	return UH_SUCCESS;
}

void pso_delete(pso_t* pso)
{
	assert(pso);

	VERB("Deleting PSO [vao: %u | sp: %u | fb: %u]\n",
		pso->vao, pso->program, pso->framebuffer
	);
	glDeleteVertexArrays(1, &(pso->vao));
	glDeleteProgram(pso->program);

	if (pso->framebuffer)
		glDeleteFramebuffers(1, &(pso->framebuffer));
}

result_t pso_create(pso_t* out_pso,
	str_t* shaders,
	vattrib_t* vertex_attribs,
	geom_t geom_type, fillmode_t fillmode, cullmode_t cullmode
)
{
	pso_t pso = {};

	glCreateVertexArrays(1, &(pso.vao));

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