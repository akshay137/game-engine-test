#ifndef UHERO_SFX_H__
#define UHERO_SFX_H__ 1

#include "../common.hpp"
#include "units.hpp"

namespace uhero::sfx
{
	typedef void (*AudioCallback)(void* userdata, u8* stream, i32 length);

	using sample_type = float;

	struct AudioBuffer
	{
		sample_type* buffer; // allocated somewhere
		i32 size;
		i32 offset; // for playback
		bool loop;

		sample_type get_sample(i32 timestep) const
		{
			i32 index = offset + timestep;
			if (index >= size)
			{
				if (!loop) return 0;
				index %= size;
			}
			return buffer[index];
		}

		// return true if ended
		bool add_offset(i32 samples)
		{
			offset += samples;
			if (offset > size)
			{
				if (!loop) return true;
				offset %= size;
			}
			return false;
		}
	};

	struct Channel
	{
		AudioBuffer buffer;
		i32 status;
		float volume;
	};

	constexpr i32 MAX_AUDIO_CHANNELS = 8;

	struct Context
	{
		AudioCallback audio_callback = nullptr;
		i32 device_id = 0;
		Channel channels[MAX_AUDIO_CHANNELS];

		u32 frequency;
		u32 samples;

		Result create(AudioCallback callback=nullptr);
		void clear();

		void pause(bool status=true);

		// call SDL_LockAudioDevice | SDL_UnlockAudioDevice
		// when adding/removing audio buffers to play

		i32 play_buffer(const AudioBuffer& buffer, float volume=1.0f);
		void stop_buffer(i32 id);
	};
}

#endif