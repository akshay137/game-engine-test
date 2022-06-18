#include "texture.hpp"
#include "gfx.hpp"
#include "../logger.hpp"

#include <glad/glad.h>

namespace uhero::gfx
{
	Result Texture::create(i32 w, i32 h, PixelFormat format,
		const void* pixels, i32 mipmaps
	)
	{
		PixelData data;
		if (!pixeldata_from_format(format, data))
		{
			UH_ERROR("unknown pixel format\n");
			return Result::InvalidArgument;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &gl_id);

		glTextureStorage2D(gl_id, mipmaps + 1, data.internal_format, w, h);
		if (pixels)
		{
			glTextureSubImage2D(gl_id, 0,
				0, 0, w, h, 
				data.channel_format, data.data_type,
				pixels
			);
		}

		this->set_swizzle(data.swizzle);
		if (pixels && mipmaps > 0)
			this->generate_mipmaps();

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

	void Texture::update_region(i32 level, i32 x, i32 y, i32 width, i32 height,
		const void* pixels
	)
	{
		PixelData data;
		if (!pixeldata_from_format(pixel_format, data))
		{
			UH_ERROR("Unknown pixel format\n");
			assert(false);
		}
		glTextureSubImage2D(gl_id, level, x, y, width, height,
			data.channel_format, data.data_type,
			pixels
		);
	}

	void Texture::bind_slot(u32 index) const
	{
		assert(gl_id);

		glBindTextureUnit(index, gl_id);
		++Context::gpu_stats.texture_switch;
	}

	void Texture::reset_slot(u32 index)
	{
		glBindTextureUnit(index, 0);
		++Context::gpu_stats.texture_switch;
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
		else if (TextureFilter::BiLinear == filter)
		{
			glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (TextureFilter::TriLinear == filter)
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

	constexpr u32 max_swizzles = static_cast<u32>(SwizzleMask::MaxSwizzleMask);
	static constexpr i32 gl_swizzles[max_swizzles] = {
		[ENUM_INT(SwizzleMask::Red)] = GL_RED,
		[ENUM_INT(SwizzleMask::Green)] = GL_GREEN,
		[ENUM_INT(SwizzleMask::Blue)] = GL_BLUE,
		[ENUM_INT(SwizzleMask::Alpha)] = GL_ALPHA,
		[ENUM_INT(SwizzleMask::Zero)] = GL_ZERO,
		[ENUM_INT(SwizzleMask::One)] = GL_ONE,
	};

	i32 Texture::swizzle_mask_to_glenum(SwizzleMask mask)
	{
		u32 index = static_cast<u32>(mask);
		if (index > max_swizzles)
		{
			UH_ERROR("Unknown SwizzleMask: %d\n", mask);
			assert(false);
			return GL_ONE;
		}

		return gl_swizzles[index];
	}

	constexpr PixelData pixel_data(u32 internal_format, u32 channel_format,
		u32 data_type, Swizzle swizzle
	)
	{
		PixelData data{};
		data.internal_format = internal_format;
		data.channel_format = channel_format;
		data.data_type = data_type,
		data.swizzle = swizzle;
		return data;
	}

	constexpr u32 max_formats = static_cast<u32>(PixelFormat::MaxPixelFormats);
	constexpr PixelData pdatas[max_formats] = {
		[ENUM_INT(PixelFormat::GREYSCALE)] = pixel_data(
			GL_R8, GL_RED, GL_UNSIGNED_BYTE, SWIZZLE_RRR1
		),

		[ENUM_INT(PixelFormat::RGB8)] = pixel_data(
			GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, SWIZZLE_RGB1
		),

		[ENUM_INT(PixelFormat::RGBA8)] = pixel_data(
			GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, SWIZZLE_RGBA
		),

		[ENUM_INT(PixelFormat::RGBA_F16)] = pixel_data(
			GL_RGBA16F, GL_RGBA, GL_FLOAT, SWIZZLE_RGBA
		),

		[ENUM_INT(PixelFormat::RGBA_F32)] = pixel_data(
			GL_RGBA32F, GL_RGBA, GL_FLOAT, SWIZZLE_RGBA
		),

		[ENUM_INT(PixelFormat::Depth24Stencil8)] = pixel_data(
			GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8,
			GL_UNSIGNED_INT_24_8, SWIZZLE_RGBA
		)
	};

	bool Texture::pixeldata_from_format(PixelFormat format, PixelData& out_pd)
	{
		u32 index = static_cast<u32>(format);
		if (index >= max_formats) return false;

		out_pd = pdatas[index];
		return true;
	}
}