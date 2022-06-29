#ifndef RECTANGLE_H__
#define RECTANGLE_H__ 1

#include <glm/vec2.hpp>

namespace game
{
	struct Rectangle
	{
		glm::vec2 position;
		glm::vec2 size;

		Rectangle() = default;
		Rectangle(glm::vec2 pos)
			: position{pos}, size{0}
		{}
		Rectangle(glm::vec2 pos, glm::vec2 size)
			: position{pos}, size{size}
		{}

		float width() const { return size.x; }
		float height() const { return size.y; }

		float left() const { return position.x - size.x * .5f; }
		float right() const { return position.x + size.x * .5f; }
		float top() const { return position.y + size.y * .5f; }
		float bottom() const { return position.y - size.y * .5f; }

		bool is_verticle() const
		{
			return size.x < size.y;
		}

		void swap_witdh_height()
		{
			size = { size.y, size.x };
		}
	};
}

#endif