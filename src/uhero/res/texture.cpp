#include "texture.hpp"
#include "../logger.hpp"

#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace uhero::res
{
	gfx::Texture load_texture(const char* file)
	{
		i32 x, y, n;
		u8* pixeldata = stbi_load(file, &x, &y, &n, 0);
		if (nullptr == pixeldata)
		{
			UH_ERROR("Failed to load image: %s\n", file);
			assert(false);
			return {};
		}

		u8* aligned_pixels = pixeldata;
		if (0 != (x * n) % 4)
		{
			UH_WARN("%s is not 4 byte aligned, manually aligning...\n", file);
			
			u32 pitch = x * n;
			u32 pad = 4 - (pitch % 4);
			u32 aligned_pitch = pitch + pad;
			u32 new_size = y * aligned_pitch;

			aligned_pixels = new u8[new_size];

			for (i32 i = 0; i < y; i++)
			{
				void* src = pixeldata + pitch * i;
				void* dst = aligned_pixels + aligned_pitch * i;
				std::memcpy(dst, src, new_size);
			}
		}

		gfx::PixelFormat format;
		if (1 == n) format = gfx::PixelFormat::GREYSCALE;
		else if (3 == n) format = gfx::PixelFormat::RGB8;
		else if (4 == n) format = gfx::PixelFormat::RGBA8;

		gfx::Texture texture {};
		auto res = texture.create(x, y, format, aligned_pixels);

		UH_VERB("Loaded texture: %s[%dx%d|%d]\n", file, x, y, n);

		if (aligned_pixels != pixeldata)
			delete[] aligned_pixels;
		else
			stbi_image_free(pixeldata);
		
		if (Result::Success != res)
		{
			UH_ERROR("Failed to create texture: %s\n", file);
			texture.clear();
		}
		
		return texture;
	}
}