#ifndef UHERO_GFX_BUFFER_H__
#define UHERO_GFX_BUFFER_H__ 1

#include "../common.hpp"

namespace uhero::gfx
{
	enum BufferType : u32
	{
		Static = 0,
		Dynaminc = 1 << 0,
	};

	enum BufferBase : u32
	{
		Uniform = 0,
		ShaderStorage = 1
	};

	struct Buffer
	{
		u32 gl_id = 0;
		u32 size = 0;

		Result create(BufferType type, u32 size, const void* data = nullptr);
		void clear();

		void update_range(u32 offset, u32 length, const void* data);
		void update(const void* data)
		{
			update_range(0, size, data);
		}

		void bind_base_range(BufferBase base, u32 index, u32 offset, u32 size);
		void bind_base(BufferBase base, u32 index)
		{
			bind_base_range(base, index, 0, size);
		}
	};

	template <typename T>
	struct TBuffer
	{
		Buffer buffer;
		u32 count;

		Result create(BufferType type, u32 count, const T* data = nullptr)
		{
			auto res = buffer.create(type, count * sizeof(T), data);
			this->count = count;
			return res;
		}

		void clear()
		{
			buffer.clear();
		}

		const Buffer& get_buffer() const
		{
			return buffer;
		}

		u32 stride() const
		{
			return sizeof(T);
		}

		void update_range(u32 first, u32 count, const T* data)
		{
			u32 offset = first * sizeof(T);
			u32 length = count * sizeof(T);
			buffer.update_range(offset, length, data);
		}

		void update(const T* data)
		{
			buffer.update(data);
		}

		void bind_base_range(BufferBase base, u32 index, u32 first, u32 count)
		{
			u32 offset = first * sizeof(T);
			u32 length = count * sizeof(T);
			buffer.bind_base_range(base, index, offset, length);
		}

		void bind_base(BufferBase base, u32 index)
		{
			buffer.bind_base(base, index);
		}
	};
}

#endif