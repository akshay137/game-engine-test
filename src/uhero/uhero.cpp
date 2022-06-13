#include "uhero.hpp"
#include "memory/memory.hpp"
#include "logger.hpp"
#include "deps.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>

namespace uhero
{
	static bool dependencies_loaded = false;

	Context Context::create_context(i32 argc, char** args)
	{
		if (!dependencies_loaded)
		{
			auto res = uhero_init_dependencies();
			if (Result::Success != res)
				return {};
			dependencies_loaded = true;
		}

		Context ctx {};

		ctx.config = Config::read_config(UHERO_CONFIG_FILE);
		Result res = ctx.parse_cmd(argc, args);
		if (Result::Success != res)
		{
			UH_WARN("Some invalid arguments were passed through command line\n");
		}
		UH_STACK_INIT(ctx.config.stack_size);

		res = Window::setup_opengl_properties();
		res = ctx.main_window.create_window(ctx.config.app_name,
			ctx.config.window_width, ctx.config.window_height,
			ctx.config.display_index,
			WindowFlags::Default | WindowFlags::Borderless
		);

		res = ctx.gfx.create(ctx.main_window, ctx.config.gl_debug);
		if (Result::Success != res)
		{
			return ctx;
		}

		ctx.should_exit = false;
		ctx.current_level = nullptr;

		DUMPI(ctx.config.display_index);
		DUMPI(ctx.config.window_width);
		DUMPI(ctx.config.window_height);
		return ctx;
	}

	Result Context::parse_cmd(i32 argc, char** args)
	{
		if (0 == argc) return Result::Success;
		
		UH_INFO("Binary: %s\n", args[0]);

		return Result::Success;
	}

	void Context::shutdown()
	{
		if (current_level)
			current_level->clear();
		Config::write_config(config, UHERO_CONFIG_FILE);

		gfx.clear();
		main_window.close();

		// clear dependencies
		if (dependencies_loaded)
			uhero_clear_dependencies();
		
		UH_STACK_CLEAR();
		UH_DUMP_ALL_ALLOCATIONS();
		global_allocator.free_all();
	}

	bool Context::is_ok() const
	{
		if (nullptr == main_window.handle) return false;
		if (nullptr == gfx.gl_context) return false;

		return true;
	}

	Result Context::set_current_level(Level* level)
	{
		if (current_level)
			current_level->clear();
		
		current_level = level;
		auto res = current_level->load(*this);
		return res;
	}

	float Context::tick()
	{
		UH_STACK_RESET();
		
		auto current = SDL_GetTicks();
		auto diff = current - ticks;
		float delta = diff / 1000.0f;
		time += delta;

		SDL_Event event {};
		while (SDL_PollEvent(&event))
		{
			if (SDL_QUIT == event.type)
			{
				should_exit = true;
				return 0.0f;
			}
		}

		if (current_level)
		{
			current_level->update(delta);
			current_level->render();
		}

		main_window.swap_buffers();

		return delta;
	}
}