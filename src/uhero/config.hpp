#ifndef UHERO_CONFIG_H__
#define UHERO_CONFIG_H__ 1

#include "common.hpp"
#include "version.hpp"

namespace uhero
{
	struct Config
	{
		Version version = Version(0, 1, 0);

		const char* app_name = "uhero";
		i32 display_index = 0;
		i32 window_width = 1280;
		i32 window_height = 720;

		bool gl_debug = true;
		bool vsync = true;

		Config() = default;

		// loads config from file,
		// values not present in the config file will be set to default
		static Config read_config(const char* config_file);
		static Result write_config(const Config& config, const char* filename);
	};
}

#endif