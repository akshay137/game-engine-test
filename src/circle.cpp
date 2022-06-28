#include "circle.hpp"
#include <glm/glm.hpp>

namespace game
{
	bool Circle::collision(const Circle& rhs) const
	{
		return (radius + rhs.radius) > glm::distance(rhs.origin, origin);
	}

	bool Circle::inside(const Circle& rhs) const
	{
		if (radius > rhs.radius) return false;
		return (rhs.radius - radius) > glm::distance(rhs.origin, origin);
	}
}