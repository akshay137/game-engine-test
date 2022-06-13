#ifndef UHERO_MEMORY_H__
#define UHERO_MEMORY_H__ 1

#include "system_allocator.hpp"
#include "stack_allocator.hpp"

namespace uhero
{
	struct Memory
	{
		static usize kilobytes_to_bytes(usize kb)
		{
			return kb * 1024;
		}

		static usize megabytes_to_bytes(usize mb)
		{
			return mb * 1024 * 1024;
		}
	};
}

#endif