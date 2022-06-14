#include "config.hpp"
#include "logger.hpp"
#include "file.hpp"
#include "memory/memory.hpp"

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
		
		const auto& tgfx = tconf["gfx"];
		config.font_size = tgfx["font_size"].value_or(16);
		config.gl_debug = tgfx["gl_debug"].value_or(true);

		const auto& tsys = tconf["system"];
		config.stack_size = tsys["stack"].value_or(Memory::megabytes_to_bytes(128));
		if (config.stack_size > Memory::megabytes_to_bytes(256))
		{
			UH_WARN("Stack size will be set to 256MB\n");
			config.stack_size = Memory::megabytes_to_bytes(256);
		}

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

		file.write_format("[gfx]\n");
		file.write_format("\tfont_size = %d\n", config.font_size);
		file.write_format("\tgl_debug = %s\n", config.gl_debug ? "true" : "false");

		file.write_format("[system]\n");
		file.write_format("\tstack = %u\n", config.stack_size);

		file.close();

		return Result::Success;
	}
}