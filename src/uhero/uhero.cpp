#include "uhero.hpp"
#include "logger.hpp"
#include "deps.hpp"

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

		res = Window::setup_opengl_properties();
		res = ctx.main_window.create_window(ctx.config.app_name,
			ctx.config.window_width, ctx.config.window_height,
			ctx.config.display_index,
			WindowFlags::Default | WindowFlags::Borderless
		);

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
		Config::write_config(config, UHERO_CONFIG_FILE);

		main_window.close();

		// clear dependencies
		if (dependencies_loaded)
			uhero_clear_dependencies();
	}

	bool Context::is_ok() const
	{
		if (nullptr == main_window.handle) return false;
		return true;
	}
}