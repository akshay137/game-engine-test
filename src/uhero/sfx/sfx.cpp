#include "sfx.hpp"
#include "../logger.hpp"

#include <cmath>
#include <SDL2/SDL_audio.h>

namespace uhero::sfx
{
	struct SDLAudioScopeLock
	{
		i32 device;

		SDLAudioScopeLock(i32 id)
			: device{id}
		{
			SDL_LockAudioDevice(device);
		}

		~SDLAudioScopeLock()
		{
			SDL_UnlockAudioDevice(device);
		}
	};

	void default_audio_callback(void* userdata, u8* stream, i32 size)
	{
		Context* ctx = reinterpret_cast<Context*>(userdata);
		sample_type* samples = reinterpret_cast<sample_type*>(stream);
		i32 length = size / sizeof(sample_type);
		SDL_memset(stream, 0, size); // silence

		float global_volume = ctx->global_volume;

		for (i32 i = 0; i < MAX_AUDIO_CHANNELS; i++)
		{
			Channel& channel = ctx->channels[i];
			if (!channel.is_playing())
				continue;
			
			i32 mix_length = std::min(length, channel.remaining());
			i32 offset = channel.offset;
			float final_volume = global_volume * channel.volume;

			for (i32 j = 0; j < mix_length; j++)
			{
				sample_type sample = channel.get_sample(offset + j);
				samples[j] += sample * final_volume;
			}

			if (channel.consume(mix_length))
				channel.stop();
		}
	}

	Result Context::create(AudioCallback callback)
	{
		auto device_count = SDL_GetNumAudioDevices(0);
		UH_INFO("System has %d audio devices\n", device_count);

		for (i32 i = 0; i < device_count; i++)
		{
			auto name = SDL_GetAudioDeviceName(i, 0);
			UH_INFO("AudioDev_%d: %s\n", i, name);
		}

		if (nullptr == callback)
			callback = default_audio_callback;
		
		SDL_AudioSpec required_spec {};
		required_spec.freq = Units::khz_to_hz(44.1);
		required_spec.format = AUDIO_F32;
		required_spec.samples = Units::samples_per_milliseconds(required_spec.freq, 4);
		required_spec.channels = 1;
		required_spec.callback = callback;
		required_spec.userdata = this;

		SDL_AudioSpec obtained_spec {};
		device_id = SDL_OpenAudioDevice(nullptr, 0,
			&required_spec, &obtained_spec,
			0
		);
		if (0 == device_id)
		{
			UHSDL_ERROR(SDL_OpenAudioDevice);
			return Result::ExternalLibraryError;
		}

		UH_INFO("Opened device: %d\n", device_id);

		if (required_spec.format != obtained_spec.format)
		{
			UH_WARN("Audio format is not same as requested\n");
		}

		UH_INFO("Device Info:\n"
			"\tFrequency: %d\n"
			"\tSamples: %d\n"
			"\tChannels: %d\n"
			"\tFormat: { Signed: %s | Float: %s | Endian: %s | Bits: %d }\n",
			obtained_spec.freq,
			obtained_spec.samples,
			obtained_spec.channels,
			SDL_AUDIO_ISSIGNED(obtained_spec.format) ? "Yes" : "No",
			SDL_AUDIO_ISFLOAT(obtained_spec.format) ? "Yes" : "No",
			SDL_AUDIO_ISBIGENDIAN(obtained_spec.format) ? "Big" : "Little",
			SDL_AUDIO_BITSIZE(obtained_spec.format)
		);
		this->frequency = obtained_spec.freq;
		this->samples = obtained_spec.samples;

		for (i32 i = 0; i < MAX_AUDIO_CHANNELS; i++)
		{
			channels[i].reset();
		}

		this->set_global_volume(1.0f);
		this->pause(false);
		return Result::Success;
	}

	void Context::clear()
	{
		SDL_PauseAudioDevice(device_id, 1);
		SDL_CloseAudioDevice(device_id);
	}

	void Context::pause(bool status)
	{
		SDL_PauseAudioDevice(device_id, status);
	}

	i32 Context::play_buffer(const AudioBuffer& buffer, float volume, bool loop)
	{
		Channel new_channel {};
		new_channel.buffer = buffer;
		new_channel.volume = volume;
		new_channel.loop = loop;
		new_channel.offset = 0;
		new_channel.status = ChannelStatus::Playing;

		SDLAudioScopeLock lock(device_id);

		// SDL_LockAudioDevice(device_id);
		for (i32 i = 0; i < MAX_AUDIO_CHANNELS; i++)
		{
			Channel& channel = channels[i];
			if (channel.is_stopped())
			{
				channel = new_channel;
				// SDL_UnlockAudioDevice(device_id);
				return i;
			}
		}

		// SDL_UnlockAudioDevice(device_id);
		return -1;
	}

	void Context::stop_buffer(i32 id)
	{
		if (id >= MAX_AUDIO_CHANNELS) return;
		if (id < 0) return;
		
		SDLAudioScopeLock lock(device_id);
		channels[id].stop();
	}

	void Context::pause_channel(i32 id, bool should_pause)
	{
		if (id < 0) return;
		if (id >= MAX_AUDIO_CHANNELS) return;

		SDLAudioScopeLock lock(device_id);
		if (channels[id].is_stopped()) return;

		if (should_pause)
			channels[id].pause();
		else
			channels[id].play();
	}

	void Context::toggle_channel(i32 id)
	{
		if (id < 0) return;
		if (id >= MAX_AUDIO_CHANNELS) return;

		SDLAudioScopeLock lock(device_id);
		if (channels[id].is_playing())
			channels[id].pause();
		else
			channels[id].play();
	}

	void Context::set_channel_volume(i32 id, float volume)
	{
		if (id < 0) return;
		if (id >= MAX_AUDIO_CHANNELS) return;

		SDLAudioScopeLock lock(device_id);
		channels[id].volume = volume;
	}
}