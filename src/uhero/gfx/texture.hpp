#ifndef UHERO_GFX_TEXTURE_H__
#define UHERO_GFX_TEXTURE_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	enum PixelFormat : u32
	{
		GREYSCALE = 0,
		RGB8,
		RGBA8,
	};

	enum SwizzleMask : i32
	{
		Red,
		Green,
		Blue,
		Alpha,
		Zero,
		One,
	};

	struct Swizzle
	{
		SwizzleMask red;
		SwizzleMask green;
		SwizzleMask blue;
		SwizzleMask alpha;

		Swizzle() = default;

		Swizzle(SwizzleMask mask)
		{
			red = mask;
			green = mask;
			blue = mask;
			alpha = mask;
		}

		Swizzle(SwizzleMask r, SwizzleMask g, SwizzleMask b, SwizzleMask a)
		{
			red = r;
			green = g;
			blue = b;
			alpha = a;
		}
	};

	struct PixelData
	{
		u32 internal_format;
		u32 channel_format;
		u32 data_type;
		Swizzle swizzle;
	};

	enum TextureFilter : u32
	{
		Nearest = 0,
		Linear = 1,
		MipmapLinear = 2,
	};

	struct Texture
	{
		u32 gl_id;
		i32 width;
		i32 height;
		i32 mipmaps;
		PixelFormat pixel_format;

		Result create(i32 w, i32 h, PixelFormat format,
			const void* pixels = nullptr,
			i32 mipmaps = 0
		);
		void clear();

		float get_aspect_ratio() const
		{
			return (float)height / width;
		}

		void bind_slot(u32 index) const;

		void generate_mipmaps();
		void set_filter(TextureFilter filter);
		void set_swizzle(const Swizzle& swizzle);

		float normalized_x(float x) const { return x / width; }
		float normalized_y(float y) const { return y / height; }

		static PixelData pixeldata_from_format(PixelFormat format);
		static i32 swizzle_mask_to_glenum(SwizzleMask mask);
	};
}

#endif