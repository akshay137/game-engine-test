#include "stack_allocator.hpp"
#include "system_allocator.hpp"
#include "../logger.hpp"

#include <cstdlib>

namespace uhero
{
	StackAllocator global_stack;

	StackGroup::StackGroup(StackAllocator& allocator)
		: allocator{allocator}
	{
		this->top = allocator.begin_group();
	}

	StackGroup::~StackGroup()
	{
		allocator.end_group(top);
	}

	Result StackAllocator::create(usize size)
	{
		pool = UH_ALLOCATE(size);
		pool_size = size;
		top = 0;
		return Result::Success;
	}

	void StackAllocator::clear()
	{
		UH_FREE(pool);
		pool = nullptr;
		top = 0;
		pool_size = 0;
	}

	void* StackAllocator::allocate(usize bytes)
	{
		if (bytes > this->get_empty_space())
		{
			UH_ERROR("no memory available: [%lu/%lu]\n", top, pool_size);
			assert(false);
			return nullptr;
		}

		void* res = (void*)((u8*)pool + top);
		top += bytes;
		return res;
	}

	void StackAllocator::reset()
	{
		top = 0;
	}
}