#ifndef UHERO_GFX_SPRITE_H__
#define UHERO_GFX_SPRITE_H__ 1

#include "../common.hpp"
#include "texture.hpp"

#include <glm/vec4.hpp>

namespace uhero::gfx
{
	// self contained sprite, holds reference to its texture
	struct Sprite
	{
		const Texture& texture;
		glm::vec4 src;

		Sprite(const Texture& texture)
			: texture{texture}
		{
			src = glm::vec4(0, 0, texture.width, texture.height);
		}

		Sprite(const Texture& texture, const glm::vec4& clip)
			: texture{texture}
		{
			src = clip;
		}
	};
}

#endif