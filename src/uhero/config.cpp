#include "config.hpp"
#include "logger.hpp"

namespace uhero
{
	Config Config::read_config(const char* config_file)
	{
		UH_INFO("Reading config from: %s\n", config_file);
		Config config {};

		return config;
	}
}