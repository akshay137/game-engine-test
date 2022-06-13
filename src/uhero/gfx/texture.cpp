#include "texture.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result Texture::create(i32 w, i32 h, PixelFormat format,
		const void* pixels, i32 mipmaps
	)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &gl_id);

		PixelData data = pixeldata_from_format(format);

		glTextureStorage2D(gl_id, mipmaps + 1, data.internal_format, w, h);
		glTextureSubImage2D(gl_id, 0,
			0, 0, w, h, 
			data.channel_format, data.data_type,
			pixels
		);

		this->set_swizzle(data.swizzle);

		this->width = w;
		this->height = h;
		this->pixel_format = format;
		this->mipmaps = mipmaps;

		return Result::Success;
	}

	void Texture::clear()
	{
		assert(gl_id);

		UH_VERB("Deleing texture: %u\n", gl_id);

		glDeleteTextures(1, &gl_id);
		gl_id = 0;
	}

	void Texture::bind_slot(u32 index) const
	{
		assert(gl_id);

		glBindTextureUnit(index, gl_id);
	}

	void Texture::generate_mipmaps()
	{
		assert(gl_id);

		glGenerateTextureMipmap(gl_id);
	}

	void Texture::set_filter(TextureFilter filter)
	{
		assert(gl_id);

		if (TextureFilter::Nearest == filter)
		{
			glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if (TextureFilter::Linear == filter)
		{
			glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (TextureFilter::MipmapLinear == filter)
		{
			glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			UH_ERROR("Unknown texture filter: %u\n", filter);
			assert(false);
		}
	}

	void Texture::set_swizzle(const Swizzle& swizzle)
	{
		assert(gl_id);
		
		GLint mask[4] = {
			swizzle_mask_to_glenum(swizzle.red),
			swizzle_mask_to_glenum(swizzle.green),
			swizzle_mask_to_glenum(swizzle.blue),
			swizzle_mask_to_glenum(swizzle.alpha)
		};
		glTextureParameteriv(gl_id, GL_TEXTURE_SWIZZLE_RGBA, mask);
	}

	i32 Texture::swizzle_mask_to_glenum(SwizzleMask mask)
	{
		if (SwizzleMask::Red == mask) return GL_RED;
		if (SwizzleMask::Green == mask) return GL_GREEN;
		if (SwizzleMask::Blue == mask) return GL_BLUE;
		if (SwizzleMask::Zero == mask) return GL_ZERO;
		if (SwizzleMask::One == mask) return GL_ONE;

		UH_ERROR("Unknown SwizzleMask: %d\n", mask);
		assert(false);
		return 0;
	}

	PixelData Texture::pixeldata_from_format(PixelFormat format)
	{
		PixelData data = {};
		if (PixelFormat::GREYSCALE == format)
		{
			data.internal_format = GL_R8;
			data.channel_format = GL_RED;
			data.data_type = GL_UNSIGNED_BYTE;
			data.swizzle = Swizzle(
				SwizzleMask::Red, SwizzleMask::Red, SwizzleMask::Red,
				SwizzleMask::One
			);
		}
		else if (PixelFormat::RGB8 == format)
		{
			data.internal_format = GL_RGB8;
			data.channel_format = GL_RGB;
			data.data_type = GL_UNSIGNED_BYTE;
			data.swizzle = Swizzle(
				SwizzleMask::Red, SwizzleMask::Green, SwizzleMask::Blue,
				SwizzleMask::One
			);
		}
		else if (PixelFormat::RGBA8 == format)
		{
			data.internal_format = GL_RGBA8;
			data.channel_format = GL_RGBA;
			data.data_type = GL_UNSIGNED_BYTE;
			data.swizzle = Swizzle(
				SwizzleMask::Red, SwizzleMask::Green, SwizzleMask::Blue,
				SwizzleMask::Alpha
			);
		}
		else
		{
			UH_ERROR("Unknown pixel format: %u\n", format);
			assert(false);
		}
		return data;
	}
}