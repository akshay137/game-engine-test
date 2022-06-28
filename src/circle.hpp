#ifndef CIRCLE_H__
#define CIRCLE_H__ 1

#include <glm/vec2.hpp>

namespace game
{
	struct Circle
	{
		glm::vec2 origin;
		float radius;

		Circle() = default;
		Circle(float radius): origin{0}, radius{radius} {}
		Circle(glm::vec2 origin, float radius): origin{origin}, radius{radius} {}

		glm::vec2 quad_size() const { return glm::vec2(radius * 2); }

		bool collision(const Circle& rhs) const;

		bool inside(const Circle& rhs) const;
	};
}

#endif