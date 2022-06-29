#ifndef GAME_UTILS_H__
#define GAME_UTILS_H__ 1

#include <cmath>
#include <glm/vec2.hpp>

namespace game
{
	struct Utils
	{
		static glm::vec2 snap_to_pixel(const glm::vec2& pos)
		{
			return { round(pos.x), round(pos.y) };
		}
	};
}

#endif