#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

// sprite
layout (location = 3) in float blend;

// glyph
layout (location = 3) in float width;
layout (location = 3, component = 1) in float edge;
layout (location = 3, component = 2) in float border_width;
layout (location = 3, component = 3) in float border_edge;
layout (location = 4) in vec4 outline_color;

layout (std140, binding = 0) uniform rstate
{
	layout (offset = 0) mat4 orthographic;
	layout (offset = 64) vec4 viewport;
};

out VS_SPRITE_OUT
{
	vec3 wpos;
	vec2 uv;
	float blend;
	vec4 color;
} sprite;

out VS_GLYPH_OUT
{
	vec2 uv;
	vec4 text_color;
	vec4 outline_color;
	vec2 widths;
	vec2 edges;
} glyph;

layout (location = 0) uniform float is_sprite; // 0 if sprite

void main()
{
	const vec4 pos4 = vec4(position, 0.0f, 1.0f);
	vec4 pos = orthographic * pos4;
	gl_Position = pos;

	if (0 == int(is_sprite))
	{
		sprite.wpos = pos4.xyz;
		sprite.uv = uv;
		sprite.blend = blend;
		sprite.color = color;
	}
	else
	{
		glyph.uv = uv;
		glyph.text_color = color;
		glyph.outline_color = outline_color;
		glyph.widths = vec2(width, border_width);
		glyph.edges = vec2(edge, border_edge);
	}

}