#include "file.hpp"
#include "logger.hpp"

#include <SDL2/SDL_rwops.h>

namespace uhero
{
	Result File::open(const char* filename, FileMode mode)
	{
		char sdl_mode[8] = { 0 };
		isize i = 0;
		if (FileMode::FRead & mode) sdl_mode[i++] = 'r';
		if (FileMode::FWrite & mode) sdl_mode[i++] = 'w';
		if (FileMode::FWrite & mode) sdl_mode[i++] = 'b';

		handle = SDL_RWFromFile(filename, sdl_mode);
		if (nullptr == handle)
		{
			UHSDL_ERROR(SDL_RWFromFile);
			return Result::IOError;
		}

		this->path = filename;
		this->mode = mode;
		this->size = SDL_RWsize(handle);
		SDL_RWseek(handle, 0, RW_SEEK_SET);

		UH_VERB("Opened File %s[%ld] @%p\n", filename, size, handle);

		return Result::Success;
	}

	void File::close()
	{
		if (nullptr == handle)
			return;
		
		UH_VERB("Closing file: %s @%p\n", path, handle);
		
		SDL_RWclose(handle);
		handle = nullptr;
	}

	Result File::seek(isize offset, SeekPosition pos)
	{
		assert(handle);
		
		auto sdl_seek = RW_SEEK_SET;
		switch (pos)
		{
			case SeekPosition::Begin: sdl_seek = RW_SEEK_SET; break;
			case SeekPosition::Current: sdl_seek = RW_SEEK_CUR; break;
			case SeekPosition::End: sdl_seek = RW_SEEK_END; break;
		}

		isize res = SDL_RWseek(handle, offset, sdl_seek);
		if (-1 == res) return Result::IOError;
		return Result::Success;
	}

	usize File::write(const void* buffer, usize length)
	{
		assert(handle);
		
		auto wrote = SDL_RWwrite(handle, buffer, 1, length);
		return wrote;
	}

	usize File::read(void* buffer, usize length)
	{
		assert(handle);
		
		auto read = SDL_RWread(handle, buffer, 1, length);
		return read;
	}

	usize File::write_format(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		char buffer[2048];
		auto bw = vsnprintf(buffer, 2048, fmt, args);
		va_end(args);
		buffer[bw] = 0;

		bw = write(buffer, bw);
		return bw;
	}

	i32 File::read_le32()
	{
		assert(handle);

		auto res = SDL_ReadLE32(handle);
		i32 num = *reinterpret_cast<i32*>(&res);
		return num;
	}

	u32 File::read_leu32()
	{
		assert(handle);

		u32 num = SDL_ReadLE32(handle);
		return num;
	}
}