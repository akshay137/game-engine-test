#include "sfx.hpp"
#include "../logger.hpp"

#include <cmath>
#include <SDL2/SDL_audio.h>

namespace uhero::sfx
{
	void default_audio_callback(void*, u8* stream, i32 size)
	{
		SDL_memset(stream, 0, size); // silence
		float* samples = reinterpret_cast<float*>(stream);
		i32 length = size / sizeof(float);
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
			"\tSamples: %d\n",
			obtained_spec.freq, obtained_spec.samples
		);

		this->pause(false);
		return Result::Success;
	}

	void Context::clear()
	{
		SDL_CloseAudioDevice(device_id);
	}

	void Context::pause(bool status)
	{
		SDL_PauseAudioDevice(device_id, status);
	}
}