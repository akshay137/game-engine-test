#ifndef UHERO_FILE_H__
#define UHERO_FILE_H__ 1

#include "common.hpp"

#include <cstdarg>

struct SDL_RWops;

namespace uhero
{
	enum FileMode : u32
	{
		FRead = 1 << 0,
		FWrite = 1 << 1,
		FBinary = 1 << 2,
	};

	enum class SeekPosition : u32
	{
		Begin = 0,
		Current = 1,
		End = 2
	};

	struct File
	{
		const char* path;
		SDL_RWops* handle;
		FileMode mode;
		isize size;

		Result open(const char* filename, FileMode mode);
		void close();

		Result seek(isize offset, SeekPosition pos);

		usize write(const void* buffer, usize length);
		usize read(void* buffer, usize length);

		usize write_format(const char* fmt, ...);

		template <typename T>
		usize write_type(const T* data, usize count = 1)
		{
			auto wrote = write(data, count * sizeof(T));
			auto wcount = wrote / sizeof(T);
			return wcount;
		}

		template <typename T>
		usize read_type(T* data, usize count = 1)
		{
			auto bread = read(data, count * sizeof(T));
			auto rcount = bread / sizeof(T);
			return rcount;
		}

		i32 read_le32();
		u32 read_leu32();
	};
}

#endif