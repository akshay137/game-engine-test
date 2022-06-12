#include "uhero.h"
#include "core/logger.h"
#include "res/texture.h"
#include "res/fontatlas.h"

#include <math.h>
#include <SDL2/SDL_scancode.h>

struct game_s
{
	texture_t diffuse;
	sprite_t sprite;
	float scale;
	vec2_t pos;
	font_t cascadia;
};

result_t game_load(uhero_t* hero, void* user)
{
	struct game_s* game = (struct game_s*)user;

	result_t res = texture_loadFromFile(&(game->diffuse), "assets/logo.png");
	game->sprite = (sprite_t) {
		._tex = &(game->diffuse),
		.clip = vec4(0, 0, game->diffuse.width, game->diffuse.height)
	};
	game->scale = 128.0f / game->diffuse.width;

	game->pos = vec2(hero->window.width * 0.5, hero->window.height * 0.5);

	res = font_loadAtlas(&(game->cascadia), "assets/cascadia.atlas");

	return res;
}

void game_clear(uhero_t* hero, void* user)
{
	struct game_s* game = (struct game_s*)user;

	texture_delete(&(game->diffuse));
	font_delete(&(game->cascadia));
}

void game_update(uhero_t* hero, void* user)
{
	struct game_s* game = (struct game_s*)user;

	game->scale = 0.25f + fabs(sin(hero->time)) * 0.5f;
	game->pos = vec2(hero->input.mouse_x,
		hero->window.height - hero->input.mouse_y
	);
	keystate_t escape = hero->input.keys[SDL_SCANCODE_ESCAPE];
	if (UHKEY_RELEASED == escape)
	{
		uhero_requestExit(hero);
		return;
	}

	const input_t* ip = &(hero->input);
	keycode_t s = SDL_SCANCODE_S;
	if (ip_keyPressed(ip, s))
		INFO("S pressed\n");
	if (ip_keyReleased(ip, s))
		INFO("S released\n");
	keystate_t state = hero->input.keys[s];
	INFO("state: %d\r", state);
	if (ip_keyDown(ip, s))
	{
		game->pos = vec2(64, 64);
	}
	if (ip_keyUp(ip, s))
	{
		game->pos = vec2(hero->window.width / 2, 0);
	}
}

void game_render(uhero_t* hero, void* user)
{
	struct game_s* game = (struct game_s*)user;

	render2d_t* r2d = &(hero->r2d);

	// texture_bindSlot(&(game->diffuse), 0);

	render2d_beginSprite(r2d, &(game->diffuse));
	for (int i = 0; i < 4; i++)
	{
		render2d_drawSpriteEx(r2d, &(game->sprite),
			vec2(game->pos.x + i * game->diffuse.width * game->scale,
				game->pos.y
			),
			game->scale, 0.0f
		);
	}
	render2d_endSprite(r2d);

	fontstyle_t style = {
		.font_size = 32 * 2,
		.text_color = color32(1, 1, 1, 1),
		.outline_color = color32(0, 0, 0, 1),
		.outline_size = 0.1f,
		.font = &(game->cascadia)
	};
	vec2_t pos = render2d_text(r2d, style, vec2(0, hero->window.height),
		str("Hellow World! dt: %.3f | %c"), hero->delta, 0
	);

	pos.x = 0;
	str_t msg = str("%d:the quick brown fox jumps over lazy dog.");
	// style.font = 0;
	style.text_color = color32(0, 1, 1, 1);
	style.outline_color = color32(1, 0, 0, 1);
	for (int i = 1; i <= 12; i++)
	{
		style.font_size = i * 8;

		vec2_t pen = render2d_text(r2d, style, pos, msg, style.font_size);
		pos.y = pen.y;
	}

	style.font_size = 64;
	style.text_color = color32(1, 1, 0, 1);
	style.outline_color = color32(0, 0, 0, 1);
	render2d_text(r2d, style, vec2(0, 64 * (64.0f / r2d->_font.line_height)),
		str("Score: %4d | time: %.2fs | %c"), hero->_ticks / 1000, hero->time, 0
	);
}

int main(int argc, char** args)
{
	uhero_t engine = {};
	result_t res = uhero_init(&engine, argc, args);
	if (UH_SUCCESS != res)
	{
		ERROR("Failed to create UHeroEngine\n");
		return 0;
	}

	INFO("Setup complete\n");
	LOG("------------------------------------------------------------------------------\n");

	struct game_s game = {};
	uhero_setLevel(&engine, &game,
		game_load, game_clear, game_update, game_render
	);
	
	while (!uhero_exitRequested(&engine))
	{
		uhero_tick(&engine);
	}

	LOG("------------------------------------------------------------------------------\n");
	INFO("Exiting\n");

	uhero_quit(&engine);
	return 0;
}