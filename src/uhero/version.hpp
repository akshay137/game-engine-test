#ifndef UHERO_VERSION_H__
#define UHERO_VERSION_H__ 1

#include "common.hpp"

namespace uhero
{
	struct Version
	{
		u32 major:10;
		u32 minor:10;
		u32 patch:12;

		Version() = default;
		Version(u32 major, u32 minor, u32 patch)
			: major{major}, minor{minor}, patch{patch}
		{}

	};
}

#endif