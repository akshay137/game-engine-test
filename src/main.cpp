#include "uhero/uhero.hpp"
#include "game.hpp"

// #include <glad/glad.h>
// #include <glm/vec2.hpp>
// #include <glm/vec3.hpp>
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>
// #include <glm/gtc/matrix_transform.hpp>

int main(int argc, char** args)
{
	uhero::Context engine = uhero::Context::create_context(argc, args);

	game::Game game{engine};
	engine.set_current_level(&game);

	while (!engine.exit_requested())
	{
		engine.tick();
	}

	engine.shutdown();
	return 0;
}