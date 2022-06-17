#include "system_allocator.hpp"
#include "../logger.hpp"

#include <cstdlib>

namespace uhero
{
	SystemAllocator global_allocator;

	void* SystemAllocator::allocate(usize bytes, const char* source, isize line)
	{
		void* res = std::malloc(bytes);
		if (nullptr == res)
		{
			perror("std::malloc");
			return res;
		}

		AllocationInfo info(res, bytes, source, line);
		allocations[current_allocations++] = info;

		return res;
	}

	void SystemAllocator::release(void* ptr)
	{
		std::free(ptr);

		for (usize i = 0; i < current_allocations; i++)
		{
			const auto& info = allocations[i];
			if (info.pointer == ptr)
			{
				allocations[i] = allocations[current_allocations - 1];
				--current_allocations;
				return;
			}
		}
	}

	void SystemAllocator::free_all()
	{
		for (usize i = 0; i < current_allocations; i++)
		{
			auto& info = allocations[i];
			std::free(info.pointer);
		}
		current_allocations = 0;
	}

	void SystemAllocator::dump_current_allocations() const
	{
		UH_INFO("Current Allocations: %lu\n", current_allocations);
		// for (const auto& info : allocations)
		for (usize i = 0; i < current_allocations; i++)
		{
			const auto& info = allocations[i];
			UH_INFO("%p [%lu] {%s:%ld}\n",
				info.pointer, info.bytes, info.source, info.line
			);
		}
	}
}