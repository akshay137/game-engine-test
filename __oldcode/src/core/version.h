#ifndef UHERO_VERSION_H__
#define UHERO_VERSION_H__ 1

#include "core.h"

struct version_s
{
	uint32_t major:10;
	uint32_t minor:10;
	uint32_t patch:12;
};

API_INLINE version_t version(uint32_t major, uint32_t minor, uint32_t patch)
{
	version_t v = {
		.major = major,
		.minor = minor,
		.patch = patch
	};
	return v;
}

#endif