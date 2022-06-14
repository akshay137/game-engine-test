#ifndef UHERO_GFX_UI_WIDGET_H__
#define UHERO_GFX_UI_WIDGET_H__ 1

#include "../../common.hpp"

namespace uhero::gfx::ui
{
	enum class WidgetType : u32
	{
		Empty = 0,
		Label,
		Button,
	};

	struct Label;
	struct Button;

	struct Widget
	{
		i32 pos_x;
		i32 pos_y;
		i32 size_x;
		i32 size_y;
		WidgetType type;

		virtual ~Widget() = default;

		void set_position(i32 x, i32 y)
		{
			pos_x = x;
			pos_y = y;
		}

		void set_size(i32 w, i32 h)
		{
			size_x = w;
			size_y = h;
		}

		template <typename T>
		T& read_as()
		{
			return *dynamic_cast<T*>(this);
		}
	};
}

#endif