#ifndef UHERO_FILE_H__
#define UHERO_FILE_H__ 1

#include "../core/core.h"

enum filemode_e
{
	UHFILE_READ = 1,
	UHFILE_WRITE = 2,
	UHFILE_BINARY = 4,
};
typedef enum filemode_e filemode_t;

struct file_s
{
	filemode_t mode;
	uint32_t _size;
	struct SDL_RWops* _rw;
};

API file_t uh_openFile(const char* filename, filemode_t mode);
API void uh_closeFile(file_t* file);

API size_t uh_readFile(file_t* file, void* buffer, uint32_t size);

API_INLINE size_t uh_readFileFull(const char* filename, void* buffer)
{
	file_t src = uh_openFile(filename, UHFILE_READ | UHFILE_BINARY);
	size_t read = uh_readFile(&src, buffer, src._size);
	uh_closeFile(&src);
	return read;
}

#endif