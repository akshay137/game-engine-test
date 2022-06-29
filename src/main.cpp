#include "uhero/uhero.hpp"
#include "game.hpp"
#include "colorswitch.hpp"
#include "pong.hpp"

#ifdef OS_WINDOWS
#include <SDL2/SDL_main.h>
#endif

int main(int argc, char** args)
{
	uhero::Context engine {};
	engine.create_context(argc, args);

	game::Game game{engine};
	engine.set_application(&game);
	
	game::Pong pong {};
	game::ColorSwitch color_switch {};

	// game.set_minigame(&pong);
	game.set_minigame(&color_switch);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}