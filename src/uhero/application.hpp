#ifndef UHERO_APPLICATION_H__
#define UHERO_APPLICATION_H__ 1

#include "common.hpp"

namespace uhero
{
	struct Context;

	struct IApplication
	{
		virtual ~IApplication() {}

		virtual Result load(Context& ctx) = 0;
		virtual void clear() = 0;
		virtual void update(float delta) = 0;
		virtual void render() = 0;
	};
}

#endif