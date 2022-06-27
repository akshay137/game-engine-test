#include "uhero/uhero.hpp"
#include "game.hpp"
#include "pong.hpp"

int main(int argc, char** args)
{
	uhero::Context engine {};
	engine.create_context(argc, args);

	game::Game game{engine};
	engine.set_application(&game);
	game::Pong pong {};
	game.set_minigame(&pong);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}