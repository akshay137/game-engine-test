#ifndef UHERO_RES_AUDIO_H__
#define UHERO_RES_AUDIO_H__ 1

#include "../sfx/audio_buffer.hpp"

namespace uhero::res
{
	sfx::AudioBuffer load_audio(const char* filename);
}

#endif