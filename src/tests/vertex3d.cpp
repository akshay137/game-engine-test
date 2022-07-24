#include "vertex3d.hpp"

using namespace uhero;

namespace tests
{
	gfx::VertexLayout Vertex3D::get_layout()
	{
		gfx::VertexLayout layout;
		layout.add_attribute(gfx::VertexAttribute::Vec3); // pos
		layout.add_attribute(gfx::VertexAttribute::Vec2); // uv
		layout.add_attribute(gfx::VertexAttribute::Vec3); // color
		layout.add_attribute(gfx::VertexAttribute::Vec3); // normal
		return layout;
	}
}