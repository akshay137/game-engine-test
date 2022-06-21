#include "audio.hpp"
#include "../memory/memory.hpp"
#include "../logger.hpp"

#include <SDL2/SDL_audio.h>

namespace uhero::res
{
	sfx::AudioBuffer load_audio(const char* filename)
	{
		UH_INFO("Loading audio: %s\n", filename);
		u8* wav_buffer = nullptr;
		u32 wav_length = 0;
		SDL_AudioSpec spec {};
		auto* res = SDL_LoadWAV(filename, &spec, &wav_buffer, &wav_length);
		if (nullptr == res)
		{
			UHSDL_ERROR(SDL_LoadWAV);
			return {};
		}

		usize bit_depth = SDL_AUDIO_BITSIZE(spec.format);
		usize sample_length = wav_length / ((bit_depth / 8) * spec.channels);
		
		sfx::AudioBuffer buffer {};
		buffer.buffer = UH_ALLOCATE_TYPE(sfx::sample_type, sample_length);
		buffer.size = sample_length;

		const float MAX_SIGNED = (1 << bit_depth) - 1;
		UH_VERB("Max Signed sample value: %f\n", MAX_SIGNED);
		for (i32 i = 0; i < buffer.size; i++)
		{
			i16* audio_buffer = reinterpret_cast<i16*>(wav_buffer);

			auto L = audio_buffer[i * spec.channels];
			float sample = L / MAX_SIGNED;

			buffer.buffer[i] = sample;
		}

		SDL_FreeWAV(wav_buffer);

		UH_VERB("Loaded audio: %s\n"
			"\tFrequency: %d\n"
			"\tSample rate: %d\n"
			"\tChannels: %d\n"
			"\tBitSize: %d | Signed: %s | Endian: %s | Float: %s\n"
			"\tSize: %u\n",
			filename,
			spec.freq, spec.samples, spec.channels,
			SDL_AUDIO_BITSIZE(spec.format),
			SDL_AUDIO_ISSIGNED(spec.format) ? "Yes" : "No",
			SDL_AUDIO_ISBIGENDIAN(spec.format) ? "Big" : "Little",
			SDL_AUDIO_ISFLOAT(spec.format) ? "Yes" : "No",
			wav_length
		);

		return buffer;
	}
}