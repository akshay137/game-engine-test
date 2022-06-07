#include "file.h"
#include "../core/logger.h"

#include <SDL2/SDL_rwops.h>

file_t uh_openFile(const char* filename, filemode_t mode)
{
	char _mode[8] = { 0 };
	uint32_t i = 0;
	if (UHFILE_READ & mode) _mode[i++] = 'r';
	if (UHFILE_WRITE & mode) _mode[i++] = 'w';
	if (UHFILE_BINARY & mode) _mode[i++] = 'b';
	_mode[i] = 0;
	VERB("Opening File %s in <%s> mode\n", filename, _mode);

	SDL_RWops* ops = SDL_RWFromFile(filename, _mode);
	if (NULL == ops)
	{
		ERROR("SDL_RWFromFile: %s\n", SDL_GetError());
		return (file_t) {};
	}
	uint32_t size = 0;
	size = (uint32_t)SDL_RWsize(ops);

	file_t file = {
		._rw = ops,
		.mode = mode,
		._size = size
	};

	DUMPPTR(file._rw);
	DUMPU(file._size);
	DUMPX(file.mode);

	return file;
}

void uh_closeFile(file_t* file)
{
	assert(file);
	assert(file->_rw);

	VERB("Closing file: %p\n", file->_rw);
	SDL_RWclose(file->_rw);
	file->_rw = NULL;
}

size_t uh_readFile(file_t* file, void* buffer, uint32_t size)
{
	assert(file);
	assert(file->_rw);
	assert(buffer);

	size_t read = SDL_RWread(file->_rw, buffer, 1, size);
	return read;
}