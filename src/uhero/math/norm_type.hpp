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

		Normalized(float v)
		{
			value = v * MAX_VALUE;
		}

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
	};
}

#endif