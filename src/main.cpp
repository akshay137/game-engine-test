#include "uhero/uhero.hpp"
#include "uhero/logger.hpp"
#include "game.hpp"
#include "test.hpp"
#include "epong/epong.hpp"
#include "tests/3d.hpp"

#include <cstring>

#ifdef OS_WINDOWS
#include <SDL2/SDL_main.h>
#endif

int main(int argc, char** args)
{
	uhero::Context engine {};
	engine.create_context(argc, args);

	game::Game game{engine};
	test::Test test = {};
	epong::Game epong = {};
	tests::T3D t3d = {};

	const char* name = "t3d";
	if (argc > 1) // check name
	{
		name = args[1];
	}

	if (0 == strcmp(name, "test"))
		engine.set_application(&test);
	else if (0 == strcmp(name, "game"))
		engine.set_application(&game);
	else if (0 == strcmp(name, "epong"))
		engine.set_application(&epong);
	else if (0 == strcmp(name, "t3d"))
		engine.set_application(&t3d);
	else
	{
		UH_ERROR("Invalid app name given: %s\n", name);
		engine.shutdown();
		return 0;
	}

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}