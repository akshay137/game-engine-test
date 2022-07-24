#ifndef TESTS_VERTEX3D_H__
#define TESTS_VERTEX3D_H__ 1

#include "../uhero/common.hpp"
#include "../uhero/gfx/pso.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace tests
{
	struct Vertex3D
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 color;
		glm::vec3 normal;

		Vertex3D() = default;

		Vertex3D(glm::vec3 pos)
			: position{pos}, uv{0}, color{1}, normal{0}
		{}

		Vertex3D(glm::vec3 pos, glm::vec2 uv)
			: position{pos}, uv{uv}, color{1}, normal{0}
		{}

		Vertex3D(glm::vec3 pos, glm::vec3 color)
			: position{pos}, uv{0}, color{color}, normal{0}
		{}

		Vertex3D(glm::vec3 pos, glm::vec3 color, glm::vec3 normal)
			: position{pos}, uv{0}, color{color}, normal{normal}
		{}

		Vertex3D(glm::vec3 pos, glm::vec2 uv, glm::vec3 color, glm::vec3 normal)
			: position{pos}, uv{uv}, color{color}, normal{normal}
		{}

		static uhero::gfx::VertexLayout get_layout();
	};
};

#endif