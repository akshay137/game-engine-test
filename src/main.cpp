#include "uhero/uhero.hpp"
#include "game.hpp"
#include "test.hpp"
#include "epong/epong.hpp"

#include <cstring>

#ifdef OS_WINDOWS
#include <SDL2/SDL_main.h>
#endif

int main(int argc, char** args)
{
	uhero::Context engine {};
	engine.create_context(argc, args);

	game::Game game{engine};
	test::Test test;
	epong::Game epong;

	const char* name = "";
	if (argc > 1) // check name
	{
		name = args[1];
	}

	if (0 == strcmp(name, "test"))
		engine.set_application(&test);
	else if (0 == strcmp(name, "game"))
		engine.set_application(&game);
	else
		engine.set_application(&epong);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}