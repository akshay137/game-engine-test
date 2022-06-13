#ifndef UHERO_MEMORY_SYSTEM_ALLOCATOR_H__
#define UHERO_MEMORY_SYSTEM_ALLOCATOR_H__ 1

#include "../common.hpp"
#include <vector>

namespace uhero
{
	struct AllocationInfo
	{
		void* pointer;
		usize bytes;
		const char* source;
		isize line;

		AllocationInfo(void* ptr, usize bytes, const char* source, isize line)
			: pointer{ptr}, bytes{bytes}, source{source}, line{line}
		{}
	};

	// gets memory from system
	// e.g, new, malloc, etc.
	struct SystemAllocator
	{
		std::vector<AllocationInfo> allocations;

		[[nodiscard]]
		void* allocate(usize bytes, const char* source=nullptr, isize line=0);
		void release(void* ptr);

		template <typename T>
		[[nodiscard]]
		T* allocate_type(usize count, const char* source=nullptr, isize line=0)
		{
			void* res = allocate(count * sizeof(T), source, line);
			return reinterpret_cast<T*>(res);
		}

		void free_all();

		usize get_current_allocations_count() const
		{
			return allocations.size();
		}

		void dump_current_allocations() const;
	};

	extern SystemAllocator global_allocator;
}

#define UH_ALLOCATE(size) uhero::global_allocator.allocate(size, __FILE__, __LINE__)
#define UH_ALLOCATE_TYPE(type, count) uhero::global_allocator.allocate_type<type>(count, __FILE__, __LINE__)
#define UH_FREE(ptr) uhero::global_allocator.release(ptr)

#define UH_DUMP_ALL_ALLOCATIONS() uhero::global_allocator.dump_current_allocations();

#endif