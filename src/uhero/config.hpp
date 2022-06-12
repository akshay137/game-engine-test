#ifndef UHERO_CONFIG_H__
#define UHERO_CONFIG_H__ 1

#include "common.hpp"
#include "version.hpp"

namespace uhero
{
	struct Config
	{
		Version version = Version(0, 1, 0);

		const char* app_name = "uhero::pong";
		i32 display_index = 0;
		i32 window_width = 1280;
		i32 window_height = 720;

		Config() = default;

		// loads config from file,
		// values not present in the config file will be set to default
		static Config read_config(const char* config_file);
	};
}

#endif