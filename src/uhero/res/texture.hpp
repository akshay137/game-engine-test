#ifndef UHERO_RES_TEXTURE_H__
#define UHERO_RES_TEXTURE_H__ 1

#include "../gfx/texture.hpp"

namespace uhero::res
{
	UHAPI gfx::Texture load_texture(const char* file);
}

#endif