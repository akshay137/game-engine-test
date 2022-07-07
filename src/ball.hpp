#ifndef BALL_H__
#define BALL_H__ 1

#include "uhero/math/circle.hpp"

namespace game
{
	struct Ball
	{
		uhero::math::Circle circle;
		glm::vec2 velocity;

		Ball() : circle{0}, velocity{0} {}
		Ball(const uhero::math::Circle& circle): circle{circle}, velocity{0} {}
		Ball(const uhero::math::Circle& circle, const glm::vec2 v)
			: circle{circle}, velocity{v}
		{}

		bool collides(const Ball& rhs) const
		{
			return circle.collision(rhs.circle);
		}

		bool collides(const uhero::math::Circle& circle) const
		{
			return this->circle.collision(circle);
		}

		bool inside(const Ball& rhs) const
		{
			return circle.inside(rhs.circle);
		}

		bool inside(const uhero::math::Circle& circle) const
		{
			return this->circle.inside(circle);
		}

		float left() const { return circle.origin.x - circle.radius; }
		float right() const { return circle.origin.x + circle.radius; }
		float top() const { return circle.origin.y + circle.radius; };
		float bottom() const { return circle.origin.y - circle.radius; }

		glm::vec2 position() const { return circle.origin; }
		glm::vec2 size() const { return circle.quad_size(); }
		float radius() const { return circle.radius; }
	};
}

#endif