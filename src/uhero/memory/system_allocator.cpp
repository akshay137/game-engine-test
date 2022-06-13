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
		allocations.push_back(info);

		return res;
	}

	void SystemAllocator::release(void* ptr)
	{
		std::free(ptr);

		for (usize i = 0; i < allocations.size(); i++)
		{
			const auto& info = allocations[i];
			if (info.pointer == ptr)
			{
				allocations[i] = allocations[allocations.size() - 1];
				allocations.pop_back();
				return;
			}
		}
	}

	void SystemAllocator::free_all()
	{
		for (auto& info : allocations)
		{
			std::free(info.pointer);
		}
		allocations.clear();
	}

	void SystemAllocator::dump_current_allocations() const
	{
		UH_INFO("Current Allocations: %lu\n", allocations.size());
		for (const auto& info : allocations)
		{
			UH_INFO("%p [%lu] {%s:%ld}\n",
				info.pointer, info.bytes, info.source, info.line
			);
		}
	}
}