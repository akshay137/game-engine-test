#ifndef UHERO_SFX_AUDIO_BUFFER_H__
#define UHERO_SFX_AUDIO_BUFFER_H__ 1

#include "../common.hpp"

namespace uhero::sfx
{
	using sample_type = float;

	struct AudioBuffer
	{
		sample_type* buffer = nullptr; // allocated somewhere
		i32 size = 0;

		void clear();
	};
}

#endif