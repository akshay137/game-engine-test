#ifndef UHERO_H__
#define UHERO_H__ 1

#include "common.hpp"
#include "config.hpp"
#include "window.hpp"
#include "input.hpp"
#include "gfx/gfx.hpp"
#include "gfx/font.hpp"
#include "level.hpp"
#include "time.hpp"

namespace uhero
{
	constexpr const char* const UHERO_CONFIG_FILE = "uhero_config.toml";
	// constexpr const char* const SYSTEM_FONT_FILE = "assets/firacode.atlas";
	constexpr const char* const SYSTEM_FONT_FILE = "assets/cascadia.atlas";

	// uhero context
	// optional, this is here just to provide convinience?
	// for initializing required systems with certain defaults + config
	struct Context
	{
		Config config;
		Window main_window;
		Input input;
		gfx::Context gfx;

		gfx::Font system_font;
		gfx::FontStyle style_normal;
		gfx::FontStyle style_info;
		gfx::FontStyle style_error;

		Clock main_clock;
		Level* current_level;

		bool should_exit;

		Result parse_cmd(i32 argc, char** args);

		void shutdown();

		// check if everything is initialized ok
		bool is_ok() const;

		Result set_current_level(Level* level);

		bool exit_requested() const { return should_exit; }
		void request_exit() { should_exit = true; }

		float tick();

		const gfx::Font& get_system_font() const { return system_font; }

		static Context create_context(i32 argc, char** args);
		static struct SystemAllocator& system_allocator();
		static struct StackAllocator& stack_allocator();
	};
}

#endif