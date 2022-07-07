#ifndef COLOR_PAD_H__
#define COLOR_PAD_H__ 1

#include "rectangle.hpp"

namespace game
{
	struct ColorPad
	{
		Rectangle rect;
		int first_color;
		int segment_count;
		int segment_length;
		glm::vec2 velocity;

		ColorPad() = default;
		ColorPad(glm::vec2 position, int segment_length)
			: rect{position}, first_color{0}, segment_count{1},
				segment_length{segment_length}, velocity{0}
		{
			rect.size.x = segment_length * segment_count;
			rect.size.y = 3;
		}

		void resegment(int segments);

		int segment_color_index(int color_index) const;

		glm::vec2 segment_size() const;
		glm::vec2 segment_position(int segment) const;

		int get_color_index_at(glm::vec2 position);

		float left() const { return rect.left(); }
		float right() const { return rect.right(); }
		float top() const { return rect.top(); }
		float bottom() const { return rect.bottom(); }
	};
}

#endif