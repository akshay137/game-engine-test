#ifndef UHERO_GFX_RENDERSTATE_H__
#define UHERO_GFX_RENDERSTATE_H__ 1

#include "../common.hpp"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uhero::gfx
{
	struct RenderState
	{
		glm::mat4 orthographic;
		glm::vec4 viewport;

		void resize(float width, float height, float near=-1, float far=1)
		{
			orthographic = glm::ortho(.0f, width, .0f, height, near, far);
			viewport = glm::vec4(.0f, .0f, width, height);
		}
	};
}

#endif