#ifndef UHERO_H__
#define UHERO_H__ 1

#include "common.hpp"
#include "config.hpp"
#include "window.hpp"
#include "input.hpp"
#include "gfx/gfx.hpp"
#include "gfx/font.hpp"
#include "sfx/sfx.hpp"
#include "application.hpp"
#include "time.hpp"

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
		Input input;
		gfx::Context gfx;

		sfx::Context audio;

		Clock main_clock;
		IApplication* app;

		bool should_exit;

		Result parse_cmd(i32 argc, char** args);

		Result create_context(i32 argc, char** args);
		void shutdown();

		// check if everything is initialized ok
		bool is_ok() const;

		Result set_application(IApplication* new_app);

		bool exit_requested() const { return should_exit; }
		void request_exit() { should_exit = true; }

		float tick();

		static struct SystemAllocator& system_allocator();
		static struct StackAllocator& stack_allocator();
	};
}

#endif