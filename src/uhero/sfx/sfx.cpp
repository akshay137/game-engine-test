#include "sfx.hpp"
#include "../logger.hpp"

#include <cmath>
#include <SDL2/SDL_audio.h>

namespace uhero::sfx
{
	float time = 0;

	void default_audio_callback(void* userdata, u8* stream, i32 size)
	{
		Context* ctx = reinterpret_cast<Context*>(userdata);
		SDL_memset(stream, 0, size); // silence

		sample_type* samples = reinterpret_cast<sample_type*>(stream);
		i32 length = size / sizeof(sample_type);

		float sample_count = ctx->samples;
		float timeframe = length / sample_count;

		for (i32 i = 0; i < length; i++)
		{
			float timestep = time + (i / sample_count);
			sample_type sample = 0;

			i32 _sc = 0;
			for (i32 c = 0; c < MAX_AUDIO_CHANNELS; c++)
			{
				Channel& channel = ctx->channels[c];
				if (0 == channel.status) continue;

				AudioBuffer& buffer = channel.buffer;
				sample += buffer.get_sample(i) * channel.volume;
				++_sc;
			}
			// if (_sc) sample /= (float)_sc;

			samples[i] = sample;
		}
		for (i32 c = 0; c < MAX_AUDIO_CHANNELS; c++)
		{
			Channel& channel = ctx->channels[c];
			if (channel.status)
			{
				auto ended = channel.buffer.add_offset(sample_count);
				if (ended) ctx->stop_buffer(c);
			}
		}


		time += timeframe;
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
		required_spec.samples = 1024;
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
			"\tChannels: %d\n",
			obtained_spec.freq,
			obtained_spec.samples,
			obtained_spec.channels
		);
		this->frequency = obtained_spec.freq;
		this->samples = obtained_spec.samples;

		for (i32 i = 0; i < MAX_AUDIO_CHANNELS; i++)
		{
			channels[i].volume = 1.0f;
			channels[i].status = 0;
		}

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

	i32 Context::play_buffer(const AudioBuffer& buffer, float volume)
	{
		for (i32 i = 0; i < MAX_AUDIO_CHANNELS; i++)
		{
			if (0 == channels[i].status)
			{
				channels[i].buffer = buffer;
				channels[i].status = 1;
				channels[i].volume = volume;
				return i;
			}
		}
		return -1;
	}

	void Context::stop_buffer(i32 id)
	{
		if (id >= MAX_AUDIO_CHANNELS) return;
		if (id < 0) return;
		
		channels[id].status = 0;
	}
}