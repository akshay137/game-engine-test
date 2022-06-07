#include <SDL2/SDL.h>
#include "core/config.h"
#include "core/logger.h"
#include "core/window.h"
#include "gfx/gfx.h"
#include "gfx/pso.h"
#include "res/file.h"

#include <glad/glad.h>

int main(int argc, char** args)
{
	DUMPI(argc);
	DUMPCSTR(args[0]);

	result_t res = uhero_init();
	if (UH_SUCCESS != res)
	{
		ERROR("uhero_init\n");
		return 0;
	}

	config_t config = config_read(UHERO_CONFIG_FILE);
	DUMPVER(config.app_version);
	DUMPSTR(config.app_name);

	window_t window = {};
	res = window_new(&window, &config);
	if (UH_SUCCESS != res)
	{
		ERROR("window_new\n");
		uhero_shutdown();
		return 0;
	}

	gfx_t gfx = {};
	res = gfx_init(&gfx, &config, &window);

	char vs[1024] = { 0 };
	char fs[1024] = { 0 };
	uh_readFileFull("assets/quad.vert", vs);
	uh_readFileFull("assets/quad.frag", fs);

	pso_t quad;
	res = pso_create(&quad,
		(str_t[]) { str(vs), str(fs), str_empty() },
		(vattrib_t[]) { VEC3, VEC3, VEC3 }, // vertex attribs
		TRIANGLES, FILL, CULL_BACK // states
	);

	INFO("Setup complete\n");
	LOG("------------------------------------------------------------------------------\n");
	
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
		} // while (SDL_PollEvent)

		float color[] = { 0.2, 0.2, 0.2, 0 };
		glClearBufferfv(GL_COLOR, 0, color);

		pso_makeCurrent(&quad);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		window_swapBuffers(&window);
	}

	LOG("------------------------------------------------------------------------------\n");
	INFO("Exiting\n");

	pso_delete(&quad);

	gfx_shutdown(&gfx);
	window_del(&window);

	// before exiting, write config to file
	config_write(&config, UHERO_CONFIG_FILE);
	uhero_shutdown();
	return 0;
}