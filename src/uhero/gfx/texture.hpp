#ifndef UHERO_GFX_TEXTURE_H__
#define UHERO_GFX_TEXTURE_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	enum PixelFormat : u32
	{
		GREYSCALE = 0,
		GREYSCALE_ALPHA,
		RGB8,
		RGBA8,

		// framebuffer related
		RGBA_F16,
		RGBA_F32,
		Depth24Stencil8,

		MaxPixelFormats,
	};

	enum SwizzleMask : i32
	{
		Red = 0,
		Green,
		Blue,
		Alpha,
		Zero,
		One,

		MaxSwizzleMask,
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

		constexpr Swizzle(SwizzleMask r, SwizzleMask g, SwizzleMask b, SwizzleMask a)
			: red{r}, green{g}, blue{b}, alpha{a}
		{}
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
		BiLinear = 2,
		TriLinear = 3,
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

		void update_region(i32 level, i32 x, i32 y, i32 width, i32 height,
			const void* pixels
		);

		float get_aspect_ratio() const
		{
			return (float)height / width;
		}

		void bind_slot(u32 index) const;
		static void reset_slot(u32 index);

		void generate_mipmaps();
		void set_filter(TextureFilter filter);
		void set_swizzle(const Swizzle& swizzle);

		float normalized_x(float x) const { return x / width; }
		float normalized_y(float y) const { return y / height; }

		static bool pixeldata_from_format(PixelFormat format, PixelData& out_pd);
		static i32 swizzle_mask_to_glenum(SwizzleMask mask);
	};

	// some constants
	constexpr Swizzle SWIZZLE_RRR1(SwizzleMask::Red, SwizzleMask::Red, SwizzleMask::Red, SwizzleMask::One);
	constexpr Swizzle SWIZZLE_RA(SwizzleMask::Red, SwizzleMask::Red, SwizzleMask::Red, SwizzleMask::Green);
	constexpr Swizzle SWIZZLE_RGB1(SwizzleMask::Red, SwizzleMask::Green, SwizzleMask::Blue, SwizzleMask::One);
	constexpr Swizzle SWIZZLE_RGBA(SwizzleMask::Red, SwizzleMask::Green, SwizzleMask::Blue, SwizzleMask::Alpha);
}

#endif