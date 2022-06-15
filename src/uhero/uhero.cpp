#include "uhero.hpp"
#include "deps.hpp"
#include "logger.hpp"
#include "memory/memory.hpp"
#include "res/font_atlas.hpp"

#include <SDL2/SDL_events.h>

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

		ctx.system_font = res::load_font(SYSTEM_FONT_FILE);
		auto style = gfx::FontStyle(ctx.config.font_size);
		ctx.style_normal = style;
		ctx.style_info = style;
		ctx.style_info.text_color = gfx::Color32::from_rgba(0, 1, 0);
		ctx.style_error = style;
		ctx.style_error.text_color = gfx::Color32::from_rgba(1, 0, 0);

		ctx.should_exit = false;
		ctx.current_level = nullptr;
		ctx.main_clock.reset();

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

		system_font.clear();
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

		main_clock.tick();
		float delta = main_clock.delta();

		input.reset();
		SDL_Event event {};
		while (SDL_PollEvent(&event))
		{
			if (SDL_QUIT == event.type)
			{
				should_exit = true;
				return 0.0f;
			}

			if (SDL_KEYUP == event.type) // released
			{
				auto scancode = event.key.keysym.scancode;
				input.set_keystate(scancode, KeyState::Released);
			}
			else if (SDL_KEYDOWN == event.type) // pressed
			{
				auto scancode = event.key.keysym.scancode;
				input.set_keystate(scancode, KeyState::Pressed);

				if (event.key.repeat)
					input.set_keystate(scancode, KeyState::Repeat);
			}
			else if (SDL_MOUSEBUTTONDOWN == event.type) // mouse pressed
			{
				auto button = event.button.button;
				// input.keys[button] &= KeyState::Pressed;
			}
			else if (SDL_MOUSEBUTTONUP == event.type) // mouse released
			{
				auto button = event.button.button;
				// input.keys[button] &= KeyState::Released;
			}
		}

		i32 nkeys = 0;
		const u8* keys = SDL_GetKeyboardState(&nkeys);
		for (i32 i = 0; i < nkeys; i++)
		{
			if (keys[i]) input.set_keystate(i, KeyState::Down);
		}

		i32 x, y;
		SDL_GetMouseState(&x, &y);
		input.mouse.x = x;
		input.mouse.y = y;

		gfx.update_render_state(main_window.width, main_window.height);
		float color[4] = { .1, .1, .1, 0 };
		gfx.clear_buffer(color, 1.0f, 0);
		if (current_level)
		{
			current_level->update(delta);
			current_level->render();
		}

		main_window.swap_buffers();

		return delta;
	}

	SystemAllocator& Context::system_allocator()
	{
		return global_allocator;
	}

	StackAllocator& Context::stack_allocator()
	{
		return global_stack;
	}
}