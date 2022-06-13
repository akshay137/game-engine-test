#include "stack_allocator.hpp"
#include "system_allocator.hpp"

#include <cstdlib>

namespace uhero
{
	StackAllocator global_stack;
	
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
		void* res = pool + top;
		top += bytes;
		return res;
	}

	void StackAllocator::reset()
	{
		top = 0;
	}
}