#include "uhero.hpp"
#include "deps.hpp"
#include "logger.hpp"
#include "memory/memory.hpp"
#include "res/font_atlas.hpp"

#include <SDL2/SDL_events.h>

namespace uhero
{
	static bool dependencies_loaded = false;

	Result Context::create_context(i32 argc, char** args)
	{
		UH_FRAME_STACK_INIT(Memory::megabytes_to_bytes(128));
		if (!dependencies_loaded)
		{
			auto res = uhero_init_dependencies();
			if (Result::Success != res)
				return {};
			dependencies_loaded = true;
		}

		config = Config::read_config(UHERO_CONFIG_FILE);
		Result res = parse_cmd(argc, args);
		if (Result::Success != res)
		{
			UH_WARN("Some invalid arguments were passed through command line\n");
		}

		res = Window::setup_opengl_properties();
		res = main_window.create_window(config.app_name,
			config.window_width, config.window_height,
			config.display_index,
			WindowFlags::Default
		);

		res = gfx.create(main_window, config.gl_debug);
		if (Result::Success != res)
		{
			return res;
		}

		res = audio.create();

		should_exit = false;
		app = nullptr;
		main_clock.reset();

		DUMPI(config.display_index);
		DUMPI(config.window_width);
		DUMPI(config.window_height);
		
		return Result::Success;
	}

	Result Context::parse_cmd(i32 argc, char** args)
	{
		if (0 == argc) return Result::Success;
		
		UH_INFO("Binary: %s\n", args[0]);

		return Result::Success;
	}

	void Context::shutdown()
	{
		if (app)
			app->clear();
		Config::write_config(config, UHERO_CONFIG_FILE);

		audio.clear();
		gfx.clear();
		main_window.close();

		// clear dependencies
		if (dependencies_loaded)
			uhero_clear_dependencies();
		
		UH_FRAME_STACK_CLEAR();
		UH_DUMP_ALL_ALLOCATIONS();
		global_allocator.free_all();
		gfx::Context::gpu_stats.dump_stats();
	}

	bool Context::is_ok() const
	{
		if (nullptr == main_window.handle) return false;
		if (nullptr == gfx.gl_context) return false;

		return true;
	}

	Result Context::set_application(IApplication* new_app)
	{
		if (app)
			app->clear();
		
		app = new_app;
		auto res = app->load(*this);
		return res;
	}

	float Context::tick()
	{
		UH_FRAME_STACK_RESET();

		main_clock.tick();
		float delta = main_clock.delta();
		if (delta > .033) delta = .033;

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
				input.set_mbuttonstate(button, KeyState::Pressed);
				// input.keys[button] &= KeyState::Pressed;
			}
			else if (SDL_MOUSEBUTTONUP == event.type) // mouse released
			{
				auto button = event.button.button - 1;
				input.set_mbuttonstate(button, KeyState::Released);
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
		auto button = SDL_GetMouseState(&x, &y);
		input.mouse.x = x;
		input.mouse.y = y;
		if (SDL_BUTTON(button) == SDL_BUTTON_LEFT)
			input.set_mbuttonstate(MouseKey::Left, KeyState::Down);


		gfx::Context::reset_stats();
		gfx.update_render_state(main_window.width, main_window.height);
		gfx.clear_buffer(gfx::Color32(0), 1.0f, 0);
		
		if (app)
		{
			app->update(delta);
			app->render();
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