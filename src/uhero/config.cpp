#include "config.hpp"
#include "logger.hpp"
#include "file.hpp"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

namespace uhero
{
	Config Config::read_config(const char* config_file)
	{
		Config config {};

		UH_INFO("Reading config from: %s\n", config_file);
		auto tconf = toml::parse_file(config_file);
		if (!tconf)
		{
			UH_ERROR("Failed to parse %s:\n", config_file);
		}

		const auto& twindow = tconf["window"];
		config.display_index = twindow["display"].value_or(0);
		config.window_width = twindow["width"].value_or(1280);
		config.window_height = twindow["height"].value_or(720);

		return config;
	}

	Result Config::write_config(const Config& config, const char* filename)
	{
		File file {};
		if (Result::Success != file.open(filename, FileMode::FWrite))
		{
			return Result::IOError;
		}

		file.write_format("[window]\n");
		file.write_format("\tdisplay = %d\n", config.display_index);
		file.write_format("\twidth = %d\n", config.window_width);
		file.write_format("\theight = %d\n", config.window_height);

		return Result::Success;
	}
}