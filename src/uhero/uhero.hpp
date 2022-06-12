#ifndef UHERO_H__
#define UHERO_H__ 1

#include "common.hpp"
#include "config.hpp"
#include "window.hpp"

namespace uhero
{
	constexpr const char* const UHERO_CONFIG_FILE = "uhero_config.toml";

	// uhero context
	// optional, this is here just to provide convinience?
	// for initializing required systems with certain defaults + config
	struct Context
	{
		Config config;
		Window main_window;

		Result parse_cmd(i32 argc, char** args);

		// clears all systems
		void shutdown();

		// check if everything is initialized ok
		bool is_ok() const;

		static Context create_context(i32 argc, char** args);
	};
}

#endif