#include "uhero.h"
#include "core/logger.h"
#include "res/texture.h"
#include "res/file.h"

#include <malloc.h>
#include <stdarg.h>
#include <math.h>
#include <SDL2/SDL_scancode.h>

/*
struct glyph_t
{
	uint32_t codepoint;
	ivec2_t pos;
	ivec2_t size;
	ivec2_t bearing;
	uint32_t advance;
};

#define MAX_GLYPHS_PER_FONT 128
struct font_t
{
	texture_t atlas;
	struct glyph_t glyphs[MAX_GLYPHS_PER_FONT];
};

struct glyph_t findGlyph(struct glyph_t* g, uint32_t code)
{
	struct glyph_t res = g[0];

	for (int32_t i = 0; i < MAX_GLYPHS_PER_FONT; i++)
	{
		if (code == g[i].codepoint)
			return g[i];
	}

	return res;
}

size_t drawText(render2d_t* render, struct font_t* font, ivec2_t pos, str_t fmt, ...)
{
	char buffer[2048] = { 0 };
	va_list args;
	va_start(args, fmt);
	size_t res = vsprintf(buffer, fmt.ptr, args);
	va_end(args);

	texture_bindSlot(&(font->atlas), 0);
	render2d_begin(render);

	vec2_t pen = vec2(pos.x, pos.y);
	int32_t line = 1;
	for (size_t i = 0; i < res; i++)
	{
		pen.y = pos.y - line * (32 + 16); // lineheight
		const char c = buffer[i];
		if (' ' == c)
		{
			pen.x += 32; // single character width
			continue;
		}
		else if ('\n' == c)
		{
			pen.x = pos.x;
			++line;
			continue;
		}
		const struct glyph_t g = findGlyph(font->glyphs, c);
		sprite_t sprite = {
			._tex = &(font->atlas),
			.clip = vec4(g.pos.x, g.pos.y, g.size.x, g.size.y)
		};

		pen.y -= (g.size.y - g.bearing.y);
		render2d_drawSprite(render, &sprite, pen);

		pen.x += g.advance;
	}

	render2d_end(render);

	return res;
}

struct font_t loadFont(const char* file)
{
	struct font_t font = {};
	file_t af = uh_openFile(file, UHFILE_READ | UHFILE_BINARY);

	uint32_t id = uh_readFileU32(&af);
	if (0x00aabb00 != id)
	{
		ERROR("Not a font atlas file: %s\n", file);
	}

	uint32_t count = uh_readFileU32(&af);
	ivec2_t size;
	size.x = uh_readFileI32(&af);
	size.y = uh_readFileI32(&af);

	INFO("%s contains %u glyphs and atlas [%d, %d]\n",
		file, count, size.x, size.y
	);

	for (uint32_t i = 0; i < count; i++)
	{
		struct glyph_t g = {};
		g.codepoint = uh_readFileU32(&af);
		g.pos.x = uh_readFileI32(&af);
		g.pos.y = uh_readFileI32(&af);
		g.size.x = uh_readFileI32(&af);
		g.size.y = uh_readFileI32(&af);
		g.bearing.x = uh_readFileI32(&af);
		g.bearing.y = uh_readFileI32(&af);
		g.advance = uh_readFileU32(&af);

		font.glyphs[i] = g;
	}

	int32_t buffer_size = size.x * size.y;
	void* tex = malloc(buffer_size);
	size_t read = uh_readFile(&af, tex, buffer_size);

	result_t res = texture_new(&(font.atlas),
		UH_GREYSCALE8, size.x, size.y, tex
	);
	free(tex);
	texture_setFiltering(&(font.atlas), UH_FILTER_LINEAR);

	uh_closeFile(&af);
	return font;
}
*/

struct game_s
{
	texture_t diffuse;
	sprite_t sprite;
	float scale;
	vec2_t pos;
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

	return res;
}

void game_clear(uhero_t* hero, void* user)
{
	struct game_s* game = (struct game_s*)user;

	texture_delete(&(game->diffuse));
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

	texture_bindSlot(&(game->diffuse), 0);

	render2d_begin(r2d);
	for (int i = 0; i < 4; i++)
	{
		render2d_drawSpriteEx(r2d, &(game->sprite),
			vec2(game->pos.x + i * game->diffuse.width * game->scale,
				game->pos.y
			),
			game->scale, 0.0f
		);
	}

	fontstyle_t style = {
		.font_size = 32,
		.text_color = color32(1, 1, 1, 1),
		.outline_color = color32(0, 0, 0, 1),
		.outline_size = 0.1f,
	};
	render2d_text(r2d, style, vec2(0, hero->window.height),
		str("Hellow World! dt: %.3f"), hero->delta
	);
	style.text_color = color32(0, 1, 1, 1);
	style.outline_color = color32(1, 1, 1, 1);
	render2d_text(r2d, style, vec2(0, hero->window.height - style.font_size),
		str("the quick brown fox jumps over lazy dog.")
	);
	style.text_color = color32(1, 0, 0, 1);
	render2d_text(r2d, style, vec2(0, hero->window.height - style.font_size * 2),
		str("THE QUICK BROWN FOX JUMPS OVER LAZY DOG.")
	);
	style.text_color = color32(0, 1, 0, 1);
	render2d_text(r2d, style, vec2(0, hero->window.height - style.font_size * 3),
		str("`~!@#$%%^&*()_+1234567890-=\n{}[]<>;:'\",./?\\|")
	);

	style.font_size = 64;
	style.text_color = color32(1, 1, 1, 1);
	render2d_text(r2d, style, vec2(0, hero->window.height - style.font_size * 3),
		str("Score: %4d | time: %.2fs"), hero->_ticks / 1000, hero->time
	);

	render2d_end(r2d);
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