#ifndef UHERO_GFX_FRAMEBUFFER_H__
#define UHERO_GFX_FRAMEBUFFER_H__ 1

#include "../common.hpp"
#include "texture.hpp"
#include "color.hpp"

namespace uhero::gfx
{
	constexpr u32 MAX_FB_COLOR_ATTACHMENTS = 8;
	struct FBDescriptor
	{
		u32 color_attachments;
		PixelFormat attachments[MAX_FB_COLOR_ATTACHMENTS];
		PixelFormat depth_attachment;

		FBDescriptor()
			: color_attachments{0},
				depth_attachment{PixelFormat::Depth24Stencil8}
		{}

		void add_color_attachment(PixelFormat format)
		{
			attachments[color_attachments] = format;
			++color_attachments;
		}

		void set_depth_attachment(PixelFormat format)
		{
			depth_attachment = format;
		}
	};

	struct FrameBuffer
	{
		u32 gl_id;
		FBDescriptor descriptor;
		Texture color[MAX_FB_COLOR_ATTACHMENTS];
		Texture depth;

		i32 width;
		i32 height;

		Result create(FBDescriptor descriptor, i32 w, i32 h);
		void clear();

		void clear_buffers(const Color32 clear_color, float depth, i32 stencil);

		bool is_ready() const;
		void make_current();
	};
}

#endif