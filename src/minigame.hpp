#ifndef MINIGAME_H__
#define MINIGAME_H__ 1

#include "uhero/common.hpp"

namespace game
{
	struct Game;

	struct MiniGame
	{
		virtual ~MiniGame() {}

		virtual bool load(Game&, int, int) { return true; }
		virtual void cleanup() {}

		virtual void reset(Game&, int, int) {}
		virtual void update(Game&, float) {}
		virtual void draw(Game&) {}

		// one time convinience function
		bool setup(Game& game, int w, int h)
		{
			if (!load(game, w, h)) return false;
			reset(game, w, h);
			return true;
		}
	};
}

#endif