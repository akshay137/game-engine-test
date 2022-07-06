#ifndef TEST_H__
#define TEST_H__ 1

#include "uhero/uhero.hpp"

#include "uhero/gfx/texture.hpp"
#include "uhero/gfx/renderer.hpp"
#include "uhero/gfx/font.hpp"
#include "gameobject.hpp"
#include "camera.hpp"
#include <glm/vec2.hpp>

#include "uhero/math/transform.hpp"

namespace test
{
	struct Test : public uhero::IApplication
	{
		uhero::gfx::Renderer renderer;
		uhero::gfx::Font font;
		uhero::gfx::Texture texture;

		/*
			root
			- c0
			- c1
			- - c10
		*/
		game::GObject root;
		game::GObject child_0;
		game::GObject child_1;
		game::GObject child_1_0;

		game::Camera2D camera;

		uhero::Context* ctx;

		uhero::Result load(uhero::Context& ctx) override;
		void clear() override;
		void update(float delta) override;
		void render() override;

		void show_debug_info();
	};
}

#endif