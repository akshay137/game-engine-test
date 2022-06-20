#include "channel.hpp"

namespace uhero::sfx
{
	void Channel::reset()
	{
		status = ChannelStatus::Stopped;
		offset = 0;
		volume = 1.0f;
		loop = false;
	}

	bool Channel::consume(i32 samples)
	{
		offset += samples;
		if (offset >= buffer.size)
		{
			if (!loop) return true;
			offset %= buffer.size;
		}
		return false;
	}

	sample_type Channel::get_sample(i32 timestep) const
	{
		i32 index = timestep % buffer.size;
		return buffer.buffer[index];
	}

	i32 Channel::remaining() const
	{
		if (loop) return buffer.size;
		return buffer.size - offset;
	}
}