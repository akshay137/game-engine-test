#ifndef UHERO_GFX_RENDER_2D_H__
#define UHERO_GFX_RENDER_2D_H__ 1

#include "vertex.h"
#include "pso.h"
#include "buffer.h"

struct sprite_s
{
	texture_t* _tex;
	vec4_t clip;
};

struct rquad_s
{
	vertex2d_t top_left;
	vertex2d_t top_right;
	vertex2d_t bottom_left;
	vertex2d_t bottom_right;
};

struct render2d_s
{
	size_t MAX_SPRITES;
	int32_t width;
	int32_t height;

	pso_t _pso;
	buffer_t _vertex_buffer;
	buffer_t _index_buffer;

	struct rquad_s* _quads;
	size_t current_sprites;
};

API result_t render2d_new(render2d_t* out_renderer,
	size_t max_sprites, int32_t width, int32_t height
);
API void render2d_delete(render2d_t* renderer);

API void render2d_begin(render2d_t* renderer);
API void render2d_end(render2d_t* renderer);

API void render2d_drawSpriteMatrix(render2d_t* renderer, sprite_t* sprite,
	mat3_t transform
);
API void render2d_drawSpriteEx(render2d_t* renderer, sprite_t* sprite,
	vec2_t pos, float scale, float angle
);
API_INLINE void render2d_drawSprite(render2d_t* renderer, sprite_t* sprite, vec2_t pos)
{
	render2d_drawSpriteEx(renderer, sprite, pos, 1.0f, 0.0f);
}


#endif