#ifndef UHERO_GFX_UI_BUTTON_H__
#define UHERO_GFX_UI_BUTTON_H__ 1

#include "widget.hpp"
#include "label.hpp"

namespace uhero::gfx::ui
{
	struct Button : public Widget
	{
		Label label;
	};
}

#endif