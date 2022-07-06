#ifndef GAME_OBJECT_H__
#define GAME_OBJECT_H__ 1

#include "uhero/math/transform.hpp"

namespace game
{
	// base game object
	struct GObject
	{
		virtual ~GObject() {}
		
		uhero::math::Transform2D transform;
		const GObject* _parent;

		GObject(): transform{}, _parent{nullptr} {}
		GObject(const uhero::math::Transform2D& transform)
			: transform{transform}, _parent{nullptr}
		{}
		GObject(const uhero::math::Transform2D& transform, const GObject& parent)
			: transform{transform}, _parent{&parent}
		{}

		void set_parent(const GObject& obj) { this->_parent = &obj; }

		void translate(const glm::vec2& amount)
		{
			transform.position += amount;
		}
		
		void scale(const glm::vec2& scale)
		{
			transform.scale *= scale;
		}

		void rotate(float angle)
		{
			transform.rotation += angle;
		}

		auto transform_matrix() const { return transform.transform_matrix(); }

		uhero::math::Transform2D world_transform() const
		{
			auto res = transform;
			if (_parent)
			{
				res = res.apply_transform(_parent->world_transform());
			}
			return res;
		}

		auto world_transform_matrix() const
		{
			return world_transform().transform_matrix();
		}
	};
}

#endif