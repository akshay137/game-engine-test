#version 450 core

in VS_SPRITE_OUT
{
	vec3 wpos;
	vec2 uv;
	float blend;
	vec4 color;
} sprite;

in VS_GLYPH_OUT
{
	vec2 uv;
	vec4 text_color;
	vec4 outline_color;
	vec2 widths;
	vec2 edges;
} glyph;

layout (location = 0) uniform float is_sprite; // 0 if sprite

layout (binding = 0) uniform sampler2D sprite_texture;
layout (binding = 1) uniform sampler2D font_texture;

layout (location = 0) out vec4 color;

void main()
{
	if (0 == int(is_sprite))
	{
		vec4 tcolor = texture(sprite_texture, sprite.uv);
		color = mix(tcolor, sprite.color, sprite.blend);
	}
	else
	{
		vec4 level = texture(font_texture, glyph.uv);
		float dist = 1.0f - level.r;

		const float twidth = glyph.widths.x;
		const float bwidth = glyph.widths.y;
		const float tedge = glyph.edges.x;
		const float bedge = glyph.edges.y;

		float a = 1.0f - smoothstep(twidth, twidth + tedge, dist);
		vec4 text_color = glyph.text_color * a;

		float ba = 1.0f - smoothstep(bwidth, bwidth + bedge, dist);
		vec4 border_color = glyph.outline_color * ba;

		float final_alpha = a + (1.0f - a) * ba;

		vec4 final_color = mix(glyph.outline_color, glyph.text_color, a / final_alpha);

		color = vec4(final_color.rgb, final_alpha);
	}
}