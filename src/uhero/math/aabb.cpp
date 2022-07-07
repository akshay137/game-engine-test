#include "aabb.hpp"

namespace uhero::math
{
	bool AABB::collides(const AABB& other) const
	{
		if (left() > other.right()) return false;
		if (right() < other.left()) return false;
		if (top() < other.bottom()) return false;
		if (bottom() > other.top()) return false;
		return true;
	}
}