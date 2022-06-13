#ifndef UHERO_LEVEL_H__
#define UHERO_LEVEL_H__ 1

#include "common.hpp"

namespace uhero
{
	struct Context;

	struct Level
	{
		virtual ~Level() {}

		virtual Result load(Context& ctx) = 0;
		virtual void clear() = 0;
		virtual void update(float delta) = 0;
		virtual void render() = 0;
	};
}

#endif