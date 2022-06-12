#ifndef UHERO_LEVEL_H__
#define UHERO_LEVEL_H__ 1

#include "common.hpp"

namespace uhero
{
	struct Context;

	struct Level
	{
		virtual ~Level() {}

		virtual Result load(Context& ctx) { return Result::Success; }
		virtual void clear(Context& ctx) {}
		virtual void update(Context& ctx, float delta) {}
		virtual void render(Context& ctx) {}
	};
}

#endif