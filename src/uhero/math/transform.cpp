#include "transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uhero::math
{
	glm::mat4 Transform2D::transform_matrix() const
	{
		glm::mat4 res(1);
		res = glm::translate(res, glm::vec3(position.x, position.y, 0.0f));
		res = glm::rotate(res, rotation, glm::vec3(0, 1, 0));
		res = glm::scale(res, glm::vec3(scale.x, scale.y, 1.0f));
		return res;
	}
}