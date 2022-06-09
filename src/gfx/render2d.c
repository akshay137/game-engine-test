#include "render2d.h"
#include "../core/logger.h"
#include "../ds/str.h"
#include "../res/file.h"

#include <malloc.h>
#include <glad/glad.h>

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

static const vertex2d_t QUAD_VERTICES[] = {
	(vertex2d_t) { {{ -0.5, 0.5 }}, {{ 0, 1 }}, {{ 1, 1, 1 }} }, // left-top
	(vertex2d_t) { {{ 0.5, 0.5 }}, {{ 1, 1 }}, {{ 1, 1, 1 }} }, // right-top
	(vertex2d_t) { {{ -0.5, -0.5 }}, {{ 0, 0 }}, {{ 1, 1, 1 }} }, // left-bottom
	(vertex2d_t) { {{ 0.5, -0.5 }}, {{ 1, 0 }}, {{ 1, 1, 1 }} }, // right-bottom
};
static const uint16_t QUAD_INDICES[] = {
	0, 1, 2,
	2, 1, 3
};

API_INLINE struct rquad_s rquad_default(void)
{
	struct rquad_s quad = {
		.top_left = QUAD_VERTICES[0],
		.top_right = QUAD_VERTICES[1],
		.bottom_left = QUAD_VERTICES[2],
		.bottom_right = QUAD_VERTICES[3]
	};
	return quad;
}

result_t render2d_new(render2d_t* out_renderer,
	size_t max_sprites, int32_t width, int32_t height
)
{
	result_t res = UH_SUCCESS;

	// create pso
	char vs[2048] = { 0 };
	char fs[2048] = { 0 };
	size_t read = 0;
	read = uh_readFileFull("assets/quad.vert", vs);
	vs[read] = 0;
	res = uh_readFileFull("assets/quad.frag", fs);
	fs[read] = 0;

	pso_t pso = {};
	res = pso_create(&pso,
		(str_t[]) { str(vs), str(fs), str_empty() },
		(vattrib_t[]) {
			UHVATTRIB_VEC2, UHVATTRIB_VEC2, UHVATTRIB_VEC3, UHVATTRIB_NONE,
		},
		UHGEOM_TRIANGLES, UHFILLMODE_FILL, UHCULLMODE_CULL_BACK
	);
	if (UH_SUCCESS != res)
	{
		ERROR("Could not create pso: %d\n", res);
		return res;
	}

	size_t buffer_size = max_sprites * sizeof(vertex2d_t) * 4;
	size_t isize = max_sprites * sizeof(uint16_t) * 6;
	void* memory = malloc(buffer_size);

	// fill indices buffer
	uint16_t* _indices = (uint16_t*)memory;
	size_t index_count = max_sprites;
	for (size_t i = 0; i < index_count; i++)
	{
		for (size_t j = 0; j < 6; j++)
			_indices[i * 6 + j] = i * 4 + QUAD_INDICES[j];
	}
	buffer_t ibuffer = buffer_new(UHBUFFER_STATIC, isize, _indices);
	buffer_t vbuffer = buffer_new(UHBUFFER_DYNAMIC, buffer_size, memory);

	*out_renderer = (render2d_t) {
		.MAX_SPRITES = max_sprites,
		.width = width,
		.height = height,
		._pso = pso,
		._vertex_buffer = vbuffer,
		._index_buffer = ibuffer,
		._quads = (struct rquad_s*)memory,
		.current_sprites = 0
	};

	return UH_SUCCESS;
}

void render2d_delete(render2d_t* renderer)
{
	assert(renderer);

	free(renderer->_quads);
	buffer_delete(&(renderer->_vertex_buffer));
	buffer_delete(&(renderer->_index_buffer));
	pso_delete(&(renderer->_pso));
}

void render2d_begin(render2d_t* renderer)
{
	assert(renderer);

	renderer->current_sprites = 0;
}

void render2d_end(render2d_t* renderer)
{
	assert(renderer);

	buffer_update(&(renderer->_vertex_buffer), renderer->_quads);
	
	pso_t* pso = &(renderer->_pso);
	pso_makeCurrent(pso);
	pso_setVertexBuffer(pso, 0, &(renderer->_vertex_buffer),
		0, sizeof(vertex2d_t)
	);
	pso_setIndexBuffer(pso, &(renderer->_index_buffer));

	uint32_t count = renderer->current_sprites * 6;
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void render2d_drawSprite(render2d_t* renderer, sprite_t* sprite, vec2_t pos)
{
	struct rquad_s quad = rquad_default();

	quad.top_left.pos = pos;

	quad.top_right.pos = vec2(pos.x + sprite->clip.z, pos.y);

	quad.bottom_left.pos = vec2(pos.x, pos.y + sprite->clip.w);

	quad.bottom_right.pos = vec2(pos.x + sprite->clip.z, pos.y + sprite->clip.w);

	size_t index = renderer->current_sprites;
	renderer->_quads[index] = quad;
	renderer->current_sprites += 1;
}

void render2d_drawSpriteEx(render2d_t* renderer, sprite_t* sprite,
	vec2_t pos, float scale, float angle
)
{
	struct rquad_s quad = rquad_default();

	quad.top_left.pos = pos;

	quad.top_right.pos = vec2(pos.x + sprite->clip.z * scale, pos.y);

	quad.bottom_left.pos = vec2(pos.x, pos.y + sprite->clip.w * scale);

	quad.bottom_right.pos = vec2(pos.x + sprite->clip.z * scale,
		pos.y + sprite->clip.w * scale
	);

	size_t index = renderer->current_sprites;
	renderer->_quads[index] = quad;
	renderer->current_sprites += 1;
}

void render2d_drawSpriteMatrix(render2d_t* renderer, sprite_t* sprite,
	mat3_t transform
)
{
	// TODO
}
