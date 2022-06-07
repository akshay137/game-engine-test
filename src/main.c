#include <SDL2/SDL.h>

int main(int argc, char** args)
{
	SDL_Log("%s | argc: %d\n", args[0], argc);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("SDL_Init: %s\n", SDL_GetError());
		return 0;
	}

	int pos = SDL_WINDOWPOS_UNDEFINED_DISPLAY(0);
	SDL_Window* window = SDL_CreateWindow("uhero::pong", pos, pos, 1280, 720, 0);
	if (NULL == window)
	{
		SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	int running = 1;
	while (running)
	{
		SDL_Event event = {};
		while (SDL_PollEvent(&event))
		{
			if (SDL_QUIT == event.type)
			{
				running = 0;
				break;
			}
			else if (SDL_KEYUP == event.type)
			{
				if (SDLK_ESCAPE == event.key.keysym.sym)
				{
					running = 0;
					break;
				}
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}