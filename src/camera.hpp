#ifndef CAMERA_H__
#define CAMERA_H__ 1

#include "uhero/math/transform.hpp"

namespace game
{
	struct Camera2D
	{
		uhero::math::Transform2D transform;
		glm::vec2 size;

		Camera2D() = default;
		Camera2D(glm::vec2 size): transform{}, size{size} {}
		Camera2D(float w, float h): transform{}, size{w, h} {}

		void move(float amount, glm::vec2 direction)
		{
			transform.position += direction * amount;
		}

		void zoom(float amount)
		{
			transform.scale += amount;
		}

		uhero::math::Transform2D view_transform(glm::vec2 view_size) const
		{
			auto res = transform;
			auto ratio = view_size / size;
			res.position = (-res.position * size) + size * .5f;
			res.position = res.position * ratio + ratio * .5f;
			res.rotation = -res.rotation;
			res.scale = res.scale * ratio;
			return res;
		}
	};
}

#endif