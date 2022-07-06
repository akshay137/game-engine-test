#ifndef UHERO_MATH_TRANSFORM_H__
#define UHERO_MATH_TRANSFORM_H__ 1

#include "../common.hpp"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace uhero::math
{
	// 2d transform
	struct Transform2D
	{
		glm::vec2 position;
		float rotation;
		glm::vec2 scale;

		Transform2D() = default;

		glm::mat4 transform_matrix() const;
	};
}

#endif