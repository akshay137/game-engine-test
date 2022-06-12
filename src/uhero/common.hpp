#ifndef UHERO_COMMON_H__
#define UHERO_COMMON_H__ 1

#include <cstdint>
#include <cstddef>

#include <assert.h>

#define UHAPI extern
#define UHINLINE static inline

namespace uhero
{
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using f32 = float;
	using f64 = double;

	enum Result : i32
	{
		Success = 0,
		Error = 1,

		ExternalLibraryError,
		NotEnoughMemoryError,
		IOError,
		NotEnoughArguments,
		InvalidArgument,
	};

	constexpr i32 OPENGL_MAJOR_VERSION = 4;
	constexpr i32 OPENGL_MINOR_VERSION = 5;
}

#endif