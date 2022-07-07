#include "transform.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uhero::math
{
	glm::vec2 rotate_vec2(glm::vec2 pos, float angle)
	{
		auto s = glm::sin(angle);
		auto c = glm::cos(angle);

		glm::vec2 res = {
			pos.x * c - pos.y * s,
			pos.x * s + pos.y * c
		};
		return res;
	}

	glm::mat4 Transform2D::transform_matrix() const
	{
		glm::mat4 res(1);
		res = glm::translate(res, glm::vec3(position.x, position.y, 0.0f));
		res = glm::rotate(res, rotation, glm::vec3(0, 1, 0));
		res = glm::scale(res, glm::vec3(scale.x, scale.y, 1.0f));
		return res;
	}

	Transform2D Transform2D::apply_transform(const Transform2D& parent) const
	{
		Transform2D res {};
		res.position = parent.position + rotate_vec2(this->position, parent.rotation) * parent.scale;
		res.rotation = this->rotation + parent.rotation;
		res.scale = this->scale * parent.scale;
		return res;
	}

	AABB Transform2D::bounding_box() const
	{
		float s = glm::abs(glm::sin(rotation));
		float c = glm::abs(glm::cos(rotation));
		auto extents = glm::vec2(scale.x * s + scale.y * c) * .5f;
		AABB box(position, extents);
		return box;
	}

	float Transform2D::radius() const
	{
		return glm::length(scale) * .5f;
	}
}