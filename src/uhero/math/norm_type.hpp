#ifndef UHERO_MATH_NORMALIZED_TYPE_H__
#define UHERO_MATH_NORMALIZED_TYPE_H__ 1

#include "../common.hpp"
#include <limits>

namespace uhero::math
{
	// convenience type for sending normalized values to OpenGL
	template <typename T>
	struct Normalized
	{
		constexpr static T MAX_VALUE = std::numeric_limits<T>::max();
		
		T value;

		Normalized() = default;
		constexpr Normalized(T v): value{v} {}
		constexpr Normalized(float v): value{static_cast<T>(v * MAX_VALUE)} {}

		template <typename E>
		constexpr Normalized(E v): value{static_cast<T>(v)} {}

		Normalized<T>& operator=(float v)
		{
			value = v * MAX_VALUE;
			return *this;
		}

		Normalized<T>& operator=(T v)
		{
			value = v;
			return *this;
		}

		float as_float() const
		{
			return value / (float)MAX_VALUE;
		}

		T invert() const
		{
			return MAX_VALUE - value;
		}
	};

	using norm8 = Normalized<i8>;
	using norm16 = Normalized<i16>;
	using norm32 = Normalized<i32>;
	using norm64 = Normalized<i64>;

	using unorm8 = Normalized<u8>;
	using unorm16 = Normalized<u16>;
	using unorm32 = Normalized<u32>;
	using unorm64 = Normalized<u64>;
}

#endif