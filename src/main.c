#include <SDL2/SDL.h>
#include "core/config.h"
#include "core/logger.h"
#include "core/window.h"
#include "gfx/gfx.h"
#include "gfx/pso.h"
#include "gfx/buffer.h"
#include "gfx/vertex.h"
#include "gfx/texture.h"
#include "res/file.h"
#include "res/texture.h"
#include "math/types.h"

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
		(vattrib_t[]) {
			UHVATTRIB_VEC2, UHVATTRIB_VEC2, UHVATTRIB_VEC3, UHVATTRIB_NONE
		},
		UHGEOM_TRIANGLES, UHFILLMODE_FILL, UHCULLMODE_CULL_BACK,
		UHFRAMEBUFFER_DEFAULT, window.width, window.height
	);

	vertex2d_t vertices[] = {
		vertex2d(vec2(0.5, 0.5), vec2(1, 0), vec3(1, 0, 0)),
		vertex2d(vec2(0.5, -0.5), vec2(1, 1), vec3(0, 1, 0)),
		vertex2d(vec2(-0.5, -0.5), vec2(0, 1), vec3(0, 0, 1)),

		vertex2d(vec2(-0.5, 0.5), vec2(0, 0), vec3(0, 1, 1)),
		vertex2d(vec2(0.5, 0.5), vec2(1, 0), vec3(1, 0, 0)),
		vertex2d(vec2(-0.5, -0.5), vec2(0, 1), vec3(0, 0, 1)),
	};
	buffer_t vbuffer = buffer_new(UHBUFFER_STATIC, sizeof(vertices), vertices);

	texture_t diffuse;
	res = texture_loadFromFile(&diffuse, "assets/logo.png");
	texture_setFiltering(&diffuse, UH_FILTER_NEAREST);

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
		pso_setVertexBuffer(&quad, 0, &vbuffer, 0, sizeof(vertex2d_t));
		texture_bindSlost(&diffuse, 0);
		pso_drawArrays(&quad, 0, 6);

		window_swapBuffers(&window);
	}

	LOG("------------------------------------------------------------------------------\n");
	INFO("Exiting\n");

	texture_delete(&diffuse);
	buffer_delete(&vbuffer);
	pso_delete(&quad);

	gfx_shutdown(&gfx);
	window_delete(&window);

	// before exiting, write config to file
	config_write(&config, UHERO_CONFIG_FILE);
	uhero_shutdown();
	return 0;
}