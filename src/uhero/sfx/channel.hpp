#ifndef UHERO_SFX_CHANNEL_H__
#define UHERO_SFX_CHANNEL_H__ 1

#include "../common.hpp"
#include "audio_buffer.hpp"

namespace uhero::sfx
{
	enum ChannelStatus : i32
	{
		Stopped = 0,
		Playing = 1,
		Paused = 2,
	};

	struct Channel
	{
		AudioBuffer buffer;
		float volume;
		ChannelStatus status;
		i32 offset;
		bool loop;

		void reset();

		// increases offset by samples
		// returns true if buffer playback should end
		bool consume(i32 samples);

		// loops over, if timestep is greater than available samples
		sample_type get_sample(i32 timestep) const;

		// if buffer is set to loop, remaining will be infinity
		// instead return buffer size for simplicity
		// if not looping, return remaining samples
		i32 remaining() const;

		bool is_playing() const { return ChannelStatus::Playing == status; }
		bool is_stopped() const { return ChannelStatus::Stopped == status; }
		bool is_paused() const { return ChannelStatus::Paused == status; }

		void play() { status = ChannelStatus::Playing; }
		void stop() { status = ChannelStatus::Stopped; offset = 0; }
		void pause() { status = ChannelStatus::Paused; }
	};
}

#endif