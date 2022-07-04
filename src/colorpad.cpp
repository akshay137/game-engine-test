#include "colorpad.hpp"
#include <glm/glm.hpp>

namespace game
{
	void ColorPad::resegment(int segments)
	{
		if (segments < 1) return;

		rect.size.x = segments * segment_length;
		segment_count = segments;
	}

	int ColorPad::segment_color_index(int color_index) const
	{
		return (first_color + color_index);
	}

	glm::vec2 ColorPad::segment_size() const
	{
		return glm::vec2(rect.width() / segment_count, rect.height());
	}
	
	glm::vec2 ColorPad::segment_position(int segment) const
	{
		glm::vec2 pos(rect.position - rect.size * .5f);
		auto size = segment_size();
		return pos + glm::vec2(size.x * segment + size.x * .5f, size.y * .5f);
	}

	int ColorPad::get_color_index_at(glm::vec2 position)
	{
		auto width = position.x - rect.left();
		auto size = segment_size();
		int id = first_color + width / size.x;
		id = glm::clamp(id, first_color, first_color + segment_count - 1);
		return id;
	}
}