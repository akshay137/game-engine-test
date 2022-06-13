#ifndef UHERO_MEMORY_STACK_ALLOCATOR_H__
#define UHERO_MEMORY_STACK_ALLOCATOR_H__ 1

#include "../common.hpp"

namespace uhero
{
	struct StackAllocator
	{
		void* pool;
		usize pool_size;
		usize top;

		Result create(usize size);
		void clear();

		[[nodiscard]]
		void* allocate(usize bytes);

		// resets stack to 0
		void reset();

		usize used() const { return top; }
		usize get_empty_size() const { return pool_size - top; }

		template <typename T>
		[[nodiscard]]
		T* allocate_type(usize count)
		{
			void* res = allocate(count * sizeof(T));
			return reinterpret_cast<T*>(res);
		}

		static StackAllocator create_from(void* ptr, usize size)
		{
			StackAllocator allocator;
			allocator.pool = ptr;
			allocator.pool_size = size;
			allocator.top = 0;
			return allocator;
		}
	};

	// A 128 MB stack, should be enough for simple games
	extern StackAllocator global_stack;
}

#define UH_STACK_ALLOCATE(size) uhero::global_stack.allocate(size)
#define UH_STACK_ALLOCATE_TYPE(type, size) uhero::global_stack.allocate_type<type>(size)
#define UH_STACK_RESET() uhero::global_stack.reset()

#define UH_STACK_INIT(size) uhero::global_stack.create(size)
#define UH_STACK_CLEAR() uhero::global_stack.clear()

#endif