#include "uhero/uhero.hpp"
#include "game.hpp"

int main(int argc, char** args)
{
	uhero::Context engine = uhero::Context::create_context(argc, args);

	game::Game game{engine};
	engine.set_application(&game);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}