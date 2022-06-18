#include "deps.hpp"
#include "logger.hpp"

#include <SDL2/SDL.h>

namespace uhero
{
	Result uhero_init_dependencies()
	{
		auto systems = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
		if (SDL_Init(systems) < 0)
		{
			UHSDL_ERROR(SDL_Init);
			return Result::ExternalLibraryError;
		}

		return Result::Success;
	}

	void uhero_clear_dependencies()
	{
		SDL_Quit();
	}
}