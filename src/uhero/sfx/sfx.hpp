#ifndef UHERO_SFX_H__
#define UHERO_SFX_H__ 1

#include "../common.hpp"
#include "units.hpp"

namespace uhero::sfx
{
	typedef void (*AudioCallback)(void* userdata, u8* stream, i32 length);

	struct AudioBuffer
	{
		float* buffer;
		i32 size;
		i32 offset; // for playback
		bool loop;
	};

	constexpr i32 MAX_AUDIO_BUFFERS = 8;

	struct Context
	{
		AudioCallback audio_callback = nullptr;
		i32 device_id = 0;
		AudioBuffer buffers[MAX_AUDIO_BUFFERS];
		i32 audio_buffer_count = 0;

		Result create(AudioCallback callback=nullptr);
		void clear();

		void pause(bool status=true);

		// call SDL_LockAudioDevice | SDL_UnlockAudioDevice
		// when adding/removing audio buffers to play
	};
}

#endif