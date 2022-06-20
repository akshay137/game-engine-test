#ifndef UHERO_SFX_H__
#define UHERO_SFX_H__ 1

#include "../common.hpp"
#include "audio_buffer.hpp"
#include "channel.hpp"
#include "units.hpp"

namespace uhero::sfx
{
	typedef void (*AudioCallback)(void* userdata, u8* stream, i32 length);

	constexpr i32 MAX_AUDIO_CHANNELS = 8;

	struct Context
	{
		// AudioCallback audio_callback = nullptr;
		i32 device_id = 0;
		float global_volume = 1.0f;
		Channel channels[MAX_AUDIO_CHANNELS];

		u32 frequency;
		u32 samples;

		Result create(AudioCallback callback=nullptr);
		void clear();

		void pause(bool status=true);

		void set_global_volume(float volume) { global_volume = volume; }

		// call SDL_LockAudioDevice | SDL_UnlockAudioDevice
		// when adding/removing audio buffers to play

		i32 play_buffer(const AudioBuffer& buffer, float volume=1.0f, bool loop=false);
		void stop_buffer(i32 id);

		void pause_channel(i32 id, bool should_pause=true);
		void toggle_channel(i32 id);

		void set_channel_volume(i32 id, float volume);
	};
}

#endif