#ifndef UHERO_GFX_COLOR_H__
#define UHERO_GFX_COLOR_H__ 1

#include "../common.hpp"
#include "../math/norm_type.hpp"

namespace uhero::gfx
{
	// RGBA Color type
	template <typename T>
	struct Color
	{
		using ntype = math::Normalized<T>;

		ntype red;
		ntype green;
		ntype blue;
		ntype alpha;

		Color() = default;
		Color(ntype v): red{v}, green{v}, blue{v}, alpha{1.0f} {}
		Color(ntype v, ntype alpha): red{v}, green{v}, blue{v}, alpha{alpha} {}
		Color(ntype r, ntype g, ntype b): red{r}, green{g}, blue{b}, alpha{1.0f} {}
		constexpr Color(ntype red, ntype green, ntype blue, ntype alpha)
			: red{red}, green{green}, blue{blue}, alpha{alpha}
		{}

		template <typename T2>
		Color<T>& operator=(const Color<T2>& rhs)
		{
			red = rhs.red.as_float();
			green = rhs.green.as_float();
			blue = rhs.blue.as_float();
			alpha = rhs.alpha.as_float();
			return *this;
		}

		Color<T> with_alpha(float new_alpha) const
		{
			Color<T> res(red, green, blue, new_alpha);
			return res;
		}

		Color<T> invert() const
		{
			Color<T> res(red.invert(), green.invert(), blue.invert(), alpha);
			return res;
		}

		void to_rgba(f32& r, f32& g, f32& b, f32& a) const
		{
			r = red.as_float();
			g = green.as_float();
			b = blue.as_float();
			a = alpha.as_float();
		}
	};

	using Color8 = Color<u8>;
	using Color16 = Color<u16>;
	using Color32 = Color<u32>;

	// constants
	constexpr Color8 WHITE = { 255, 255, 255, 255 };
	constexpr Color8 BLACK = { 0, 0, 0,  255 };
	constexpr Color8 RED = { 255, 0, 0, 255 };
	constexpr Color8 GREEN = { 0, 255, 0, 255 };
	constexpr Color8 BLUE = { 0, 0, 255, 255 };
	constexpr Color8 CYAN = { 0, 255, 255, 255 };
	constexpr Color8 PINK = { 255, 0, 255, 255 };
	constexpr Color8 YELLOW = { 255, 255, 0, 255 };
}

#endif