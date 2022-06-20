#include "audio_buffer.hpp"
#include "../memory/memory.hpp"

namespace uhero::sfx
{
	void AudioBuffer::clear()
	{
		assert(buffer);

		UH_FREE(buffer);
		buffer = nullptr;
	}
}