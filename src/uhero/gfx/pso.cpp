#include "pso.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include "gfx.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	const u32 TRIANGLES = GL_TRIANGLES;
	
	Result PSO::create(const VertexLayout& layout,
		const char* vertex_shader, const char* fragment_shader,
		BlendState blend, DepthState depth
	)
	{
		vao = Utils::create_vao_from_layout(layout);
		program = Utils::create_program_from_vf(vertex_shader, fragment_shader);

		this->layout = layout;
		this->blend = blend;
		this->depth = depth;

		set_blendstate(blend);
		set_depthstate(depth);

		UH_VERB("Created PSO [V: %u | P: %u]\n", vao, program);

		return Result::Success;
	}

	void PSO::clear()
	{
		UH_VERB("Deleting PSO [V: %u | P: %u]\n", vao, program);

		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);

		Context::deleted_shader();

		vao = 0;
		program = 0;
	}

	void PSO::make_current()
	{
		if (current_program != program)
			glUseProgram(program);
		if (current_vao != vao)
			glBindVertexArray(vao);
		
		current_program = program;
		current_vao = vao;
		set_blendstate(blend);
		set_depthstate(depth);
	}

	void PSO::set_vertex_buffer(const Buffer& vbuffer, u32 index,
		u32 offset, u32 stride
	)
	{
		glVertexArrayVertexBuffer(vao, index, vbuffer.gl_id, offset, stride);
	}

	void PSO::set_index_buffer(const Buffer& ibuffer)
	{
		glVertexArrayElementBuffer(vao, ibuffer.gl_id);
	}

	void PSO::set_float(i32 index, f32 value)
	{
		glProgramUniform1f(program, index, value);
	}

	void PSO::set_int(i32 index, i32 value)
	{
		glProgramUniform1i(program, index, value);
	}

	void PSO::set_mat4(i32 index, f32* matrix)
	{
		glProgramUniformMatrix4fv(program, index, 1, GL_FALSE, matrix);
	}

	void PSO::set_blendstate(const BlendState state)
	{
		if (state == current_blend_state)
			return;
		current_blend_state = state;
		if (BlendState::NoBlend == state)
		{
			glDisable(GL_BLEND);
			return;
		}
		else
		{
			glEnable(GL_BLEND);
		}

		switch (state)
		{
			case BlendState::OneMinusSrcAlpha:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				return;
			
			default:
				UH_WARN("Invalid BlendState: %u\n", (u32)state);
				return;
		}
	}

	void PSO::set_depthstate(const DepthState state)
	{
		if (state == current_depth_state)
			return;
		current_depth_state = state;
		if (DepthState::NoDepth == state)
		{
			glDisable(GL_DEPTH_TEST);
			return;
		}
		else
		{
			glEnable(GL_DEPTH_TEST);
		}

		switch (state)
		{
			case DepthState::Less:
				glDepthFunc(GL_LESS);
				return;
			
			case DepthState::LessEqual:
				glDepthFunc(GL_LEQUAL);
				return;
			
			default:
				UH_WARN("Invalid DepthState: %u\n", (u32)state);
				return;
		}
	}

	void PSO::draw_elements(u32 primitive, u32 count, const void* offset)
	{
		glDrawElements(primitive, count, GL_UNSIGNED_SHORT, offset);
		Context::gpu_stats.triangle_count += (count / 3);
		++Context::gpu_stats.draw_calls;
	}

	void PSO::draw_arrays(u32 primitive, u32 first, u32 count)
	{
		glDrawArrays(primitive, first, count);
		Context::gpu_stats.triangle_count += (count / 3);
		++Context::gpu_stats.draw_calls;
	}
}