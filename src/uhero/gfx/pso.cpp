#include "pso.hpp"
#include "utils.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result PSO::create(const VertexLayout& layout,
		const char* vertex_shader, const char* fragment_shader
	)
	{
		vao = Utils::create_vao_from_layout(layout);
		program = Utils::create_program_from_vf(vertex_shader, fragment_shader);

		this->layout = layout;

		UH_VERB("Created PSO [V: %u | P: %u]\n", vao, program);

		return Result::Success;
	}

	void PSO::clear()
	{
		UH_VERB("Deleting PSO [V: %u | P: %u]\n", vao, program);
		
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);

		vao = 0;
		program = 0;
	}

	void PSO::make_current()
	{
		glUseProgram(program);
		glBindVertexArray(vao);
	}
}