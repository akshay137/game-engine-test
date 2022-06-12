#include "deps.hpp"
#include "logger.hpp"

#include <SDL2/SDL.h>

namespace uhero
{
	Result uhero_init_dependencies()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
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