#ifndef UHERO_GFX_UI_LABEL_H__
#define UHERO_GFX_UI_LABEL_H__ 1

#include "widget.hpp"
#include "../font.hpp"

#include <string>

namespace uhero::gfx::ui
{
	struct Label : public Widget
	{
		const Font& font;
		FontStyle style;
		std::string text;

		Label(const std::string& text, const Font& font)
			: text{text}, font{font}
		{}
	};
}

#endif