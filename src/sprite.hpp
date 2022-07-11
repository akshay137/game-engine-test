#ifndef SPRITE_H__
#define SPRITE_H__ 1

#include "gameobject.hpp"
#include "uhero/gfx/color.hpp"
#include "uhero/gfx/texture.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace game
{
	struct Sprite : public GObject
	{
		uhero::gfx::Color8 color;
		const uhero::gfx::Texture* texture;
		glm::vec4 clip;
		glm::vec2 size;

		Sprite(): color{255}, texture{nullptr}, clip{0}, size{0} {}

		Sprite(const uhero::gfx::Texture& tex)
			: color{255}, texture{&tex}, clip{0, 0, tex.width, tex.height},
				size{tex.width, tex.height}
		{}

		Sprite(const uhero::gfx::Texture& tex, const glm::vec2& size):
			color{255}, texture{&tex}, clip{0, 0, tex.width, tex.height},
				size{size}
		{}

		operator const uhero::gfx::Texture& () const
		{
			return *texture;
		}
	};
}

#endif