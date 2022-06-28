#ifndef MINIGAME_H__
#define MINIGAME_H__ 1

#include "uhero/common.hpp"

namespace game
{
	struct Game;

	struct MiniGame
	{
		virtual ~MiniGame() {}

		virtual bool setup(Game&, int, int) { return true; }
		virtual void update(Game&, float) {}
		virtual void draw(Game&) {}

		virtual void cleanup() {}
	};
}

#endif