#ifndef UHERO_STR_H__
#define UHERO_STR_H__ 1

#include "../core/core.h"

#include <string.h>

// sized string view
struct str_s
{
	const char* ptr;
	uintptr_t _size;
};

API_INLINE str_t str(const char* string)
{
	str_t s = {
		.ptr = string,
		._size = strlen(string)
	};
	return s;
}

API_INLINE str_t str_empty(void)
{
	str_t s = {
		.ptr = NULL,
		._size = 0
	};
	return s;
}

#endif