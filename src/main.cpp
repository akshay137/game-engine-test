#include <cstdint>
#include <cstdlib>
#include <SDL2/SDL.h>

#include <glm/vec3.hpp>

void uh_exit()
{
	SDL_Quit();
}

int main(int argc, char** args)
{
	std::atexit(uh_exit);
	
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_Log("SDL_Init: %s\n", SDL_GetError());
		return EXIT_SUCCESS;
	}

	SDL_Window* window = SDL_CreateWindow("uhero::pong test", 0, 0,
		1280, 720, SDL_WINDOW_OPENGL
	);
	if (nullptr == window)
	{
		SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
		return EXIT_SUCCESS;
	}

	bool running = true;
	while (running)
	{
		SDL_Event event {};
		while (SDL_PollEvent(&event))
		{
			if (SDL_QUIT == event.type)
			{
				running = false;
				break;
			}
			if (SDL_KEYUP == event.type)
			{
				if (SDLK_ESCAPE == event.key.keysym.sym)
				{
					running = false;
					break;
				}
			}
		}
	}

	SDL_DestroyWindow(window);
	uh_exit();
	return EXIT_SUCCESS;
}