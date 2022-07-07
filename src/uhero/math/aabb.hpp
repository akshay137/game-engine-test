#ifndef UHERO_MATH_AABB_H__
#define UHERO_MATH_AABB_H__ 1

#include "../common.hpp"
#include <glm/vec2.hpp>

namespace uhero::math
{
	struct AABB
	{
		glm::vec2 position;
		glm::vec2 extents;

		AABB() = default;
		AABB(glm::vec2 pos, glm::vec2 ext): position{pos}, extents{ext} {}

		float left() const { return position.x - extents.x; }
		float right() const { return position.x + extents.x; }
		float top() const { return position.y + extents.y; }
		float bottom() const { return position.y - extents.y; }

		bool collides(const AABB& other) const;
	};
}

#endif