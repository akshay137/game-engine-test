#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 quad_uv;
layout (location = 3) in vec4 color;

// sprite
layout (location = 4) in float blend;
layout (location = 4, component = 1) in float circle;

// glyph
layout (location = 4) in float width;
layout (location = 4, component = 1) in float edge;
layout (location = 4, component = 2) in float border_width;
layout (location = 4, component = 3) in float border_edge;
layout (location = 5) in vec4 outline_color;

layout (std140, binding = 0) uniform rstate
{
	layout (offset = 0) mat4 orthographic;
	layout (offset = 64) vec4 viewport;
};

struct Sprite
{
	float blend;
	float circle;
	vec4 color;
};

struct Glyph
{
	vec4 text_color;
	vec4 outline_color;
	vec2 widths;
	vec2 edges;
};

out VS_OUT
{
	vec2 uv;
	vec2 quad_uv;
	Sprite sprite;
	Glyph glyph;
} vs;

void main()
{
	const vec4 pos4 = vec4(position, 0.0f, 1.0f);
	vec4 pos = orthographic * pos4;
	gl_Position = pos;

	vs.uv = uv;
	vs.quad_uv = quad_uv;
	
	vs.sprite.blend = blend;
	vs.sprite.color = color;
	vs.sprite.circle = circle;
	
	vs.glyph.text_color = color;
	vs.glyph.outline_color = outline_color;
	vs.glyph.widths = vec2(width, border_width);
	vs.glyph.edges = vec2(edge, border_edge);
}