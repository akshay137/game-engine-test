#include "render2d.h"
#include "texture.h"
#include "../core/logger.h"
#include "../ds/str.h"
#include "../res/file.h"
#include "../res/fontatlas.h"

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

API_INLINE struct tquad_s tquad_default(fontstyle_t* style, uint32_t lineheight)
{
	float width = 0.5f;
	float edge = 0.1f;
	vertexglyph_t v = {
		.text_color = color32Swap(style->text_color),
		.outline_color = color32Swap(style->outline_color),
		.width = width,
		.edge = edge
	};
	struct tquad_s quad = {
		.top_left = v,
		.top_right = v,
		.bottom_left = v,
		.bottom_right = v
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

	read = uh_readFileFull("assets/font.vert", vs);
	vs[read] = 0;
	read = uh_readFileFull("assets/font.frag", fs);
	fs[read] = 0;

	pso_t font_pso = {};
	res = pso_create(&font_pso,
		(str_t[]) { str(vs), str(fs), str_empty() },
		(vattrib_t[]) {
			UHVATTRIB_VEC2, UHVATTRIB_VEC2,
			UHVATTRIB_BVEC4_NORM, UHVATTRIB_BVEC4_NORM,
			UHVATTRIB_VEC2,
			UHVATTRIB_NONE
		},
		UHGEOM_TRIANGLES, UHFILLMODE_FILL, UHCULLMODE_CULL_BACK
	);
	if (UH_SUCCESS != res)
	{
		ERROR("Could not create pso for font: %d\n", res);
		return res;
	}
	font_t font = {};
	res = font_loadAtlas(&font, "assets/font.atlas");
	if (UH_SUCCESS != res)
	{
		ERROR("Could not load font atlas: %d\n", res);
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

	uint32_t tvbuffer_size = max_sprites * sizeof(vertexglyph_t);
	void* textquads = malloc(tvbuffer_size);
	buffer_t tvbuffer = buffer_new(UHBUFFER_DYNAMIC, tvbuffer_size, textquads);

	*out_renderer = (render2d_t) {
		.MAX_SPRITES = max_sprites,
		.width = width,
		.height = height,
		._font = font,
		._sprite_pso = pso,
		._text_pso = font_pso,
		._spritevertex_buffer = vbuffer,
		._textvertex_buffer = tvbuffer,
		._index_buffer = ibuffer,
		._quads = (struct rquad_s*)memory,
		._textquads = (struct tquad_s*)textquads,
		.current_sprites = 0
	};

	return UH_SUCCESS;
}

void render2d_delete(render2d_t* renderer)
{
	assert(renderer);

	free(renderer->_quads);
	free(renderer->_textquads);
	buffer_delete(&(renderer->_textvertex_buffer));
	buffer_delete(&(renderer->_spritevertex_buffer));
	buffer_delete(&(renderer->_index_buffer));
	pso_delete(&(renderer->_sprite_pso));
	pso_delete(&(renderer->_text_pso));
	font_delete(&(renderer->_font));
}

void render2d_begin(render2d_t* renderer)
{
	assert(renderer);

	render2d_beginSprite(renderer);
	render2d_beginText(renderer);
}

void render2d_end(render2d_t* renderer)
{
	assert(renderer);

	render2d_endSprite(renderer);
	render2d_endText(renderer);
}

void render2d_beginSprite(render2d_t* renderer)
{
	assert(renderer);

	renderer->current_sprites = 0;
}

void render2d_endSprite(render2d_t* renderer)
{
	assert(renderer);

	buffer_update(&(renderer->_spritevertex_buffer), renderer->_quads);
	
	pso_t* pso = &(renderer->_sprite_pso);
	pso_makeCurrent(pso);
	pso_setVertexBuffer(pso, 0, &(renderer->_spritevertex_buffer),
		0, sizeof(vertex2d_t)
	);
	pso_setIndexBuffer(pso, &(renderer->_index_buffer));

	uint32_t count = renderer->current_sprites * 6;
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void render2d_drawSpriteEx(render2d_t* renderer, sprite_t* sprite,
	vec2_t pos, float scale, float angle
)
{
	struct rquad_s quad = rquad_default();
	const ivec2_t tsize = ivec2(sprite->_tex->width, sprite->_tex->height);
	const vec4_t clip = sprite->clip;

	quad.top_left.pos = pos;
	quad.top_left.uv = vec2(clip.x / tsize.x, (clip.y + clip.w) / tsize.y);

	quad.top_right.pos = vec2(pos.x + sprite->clip.z * scale, pos.y);
	quad.top_right.uv = vec2(
		(clip.x + clip.z) / tsize.x,
		(clip.y + clip.w) / tsize.y
	);

	quad.bottom_left.pos = vec2(pos.x, pos.y + sprite->clip.w * scale);
	quad.bottom_left.uv = vec2(clip.x / tsize.x, clip.y / tsize.y);

	quad.bottom_right.pos = vec2(pos.x + sprite->clip.z * scale,
		pos.y + sprite->clip.w * scale
	);
	quad.bottom_right.uv = vec2((clip.x + clip.z) / tsize.x, clip.y / tsize.y);

	size_t index = renderer->current_sprites;
	renderer->_quads[index] = quad;
	renderer->current_sprites += 1;

	if (renderer->current_sprites > renderer->MAX_SPRITES)
		render2d_end(renderer);
}

void render2d_drawSpriteMatrix(render2d_t* renderer, sprite_t* sprite,
	mat3_t transform
)
{
	// TODO
}


void render2d_beginText(render2d_t* renderer)
{
	assert(renderer);
	renderer->current_texquads = 0;
}

void render2d_endText(render2d_t* renderer)
{
	assert(renderer);

	font_t* fnt = &(renderer->_font);
	pso_t* pso = &(renderer->_text_pso);

	buffer_update(&(renderer->_textvertex_buffer), renderer->_textquads);

	texture_bindSlot(&(fnt->bitmap), 1);
	pso_makeCurrent(pso);
	pso_setVertexBuffer(pso, 0,
		&(renderer->_textvertex_buffer), 0, sizeof(vertexglyph_t)
	);
	pso_setIndexBuffer(pso, &(renderer->_index_buffer));

	uint32_t count = renderer->current_texquads * 6;
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void render2d_drawGlyph(render2d_t* renderer, fontstyle_t* style,
	vec2_t pos, glyph_t* glyph
)
{
	struct tquad_s quad = tquad_default(style, renderer->_font.line_height);

	font_t* fnt = &renderer->_font;
	const ivec2_t tsize = ivec2(fnt->bitmap.width, fnt->bitmap.height);
	const vec4_t clip = vec4(glyph->position.x, glyph->position.y,
		glyph->size.x,
		glyph->size.y
	);
	float scale = style->font_size / (float)fnt->line_height;

	quad.top_left.pos = pos;
	quad.top_left.uv = vec2(clip.x / tsize.x, (clip.y + clip.w) / tsize.y);

	quad.top_right.pos = vec2(pos.x + glyph->size.x * scale, pos.y);
	quad.top_right.uv = vec2(
		(clip.x + clip.z) / tsize.x,
		(clip.y + clip.w) / tsize.y
	);

	quad.bottom_left.pos = vec2(pos.x, pos.y + glyph->size.y * scale);
	quad.bottom_left.uv = vec2(clip.x / tsize.x, clip.y / tsize.y);

	quad.bottom_right.pos = vec2(pos.x + glyph->size.x * scale,
		pos.y + glyph->size.y * scale
	);
	quad.bottom_right.uv = vec2((clip.x + clip.z) / tsize.x, clip.y / tsize.y);

	size_t index = renderer->current_texquads;
	renderer->_textquads[index] = quad;
	renderer->current_texquads += 1;

	if (renderer->current_texquads > renderer->MAX_SPRITES)
		render2d_endText(renderer);
}

size_t render2d_text(render2d_t* renderer, fontstyle_t style, vec2_t pos,
	str_t fmt, ...
)
{
	char buffer[2048] = { 0 };
	va_list args;
	va_start(args, fmt);
	size_t count = vsnprintf(buffer, 2048, fmt.ptr, args);
	va_end(args);

	font_t* fnt = &(renderer->_font);

	float scale = style.font_size / (float)fnt->line_height;
	vec2_t pen = pos;
	int32_t line = 1;
	for (size_t i = 0; i < count; i++)
	{
		pen.y = pos.y - line * fnt->line_height * scale;

		const char c = buffer[i];
		if (' ' == c)
		{
			pen.x += fnt->space * scale;
			continue;
		}
		else if ('\n' == c)
		{
			line++;
			pen.x = pos.x;
			continue;
		}
		else if ('\t' == c)
		{
			pen.x += (fnt->space * 4 * scale);
			continue;
		}
		else
		{
			// drawable glyph
			glyph_t g = font_findGlyph(fnt, c);

			pen.y -= (g.size.y - g.bearing.y) * scale;
			render2d_drawGlyph(renderer, &style, pen, &g);

			pen.x += (g.advance) * scale;
		}
	}

	return count;
}
