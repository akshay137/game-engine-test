#version 450 core

in VS_SPRITE_OUT
{
	vec3 wpos;
	vec2 uv;
	vec2 quad_uv;
	float blend;
	float circle;
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

layout (location = 0) uniform int program_mode; // 0 if sprite

layout (binding = 0) uniform sampler2D sprite_texture;
layout (binding = 1) uniform sampler2D font_texture;

layout (location = 0) out vec4 color;

const int UPM_COLOR = 0;
const int UPM_SPRITE = 1;
const int UPM_GLYPH = 2;

float in_circle(float alpha)
{
	float dist = distance(vec2(0.5f), sprite.quad_uv);
	float inside = step(dist, 0.5f);
	float should_round = step(sprite.circle, 0.0f);
	return mix(inside, alpha, should_round);
	return inside * should_round + alpha * (1.0f - should_round);
}

void main()
{
	switch (program_mode)
	{
		case UPM_COLOR:
		{
			color = sprite.color;
			color.a = in_circle(color.a);
		}
			break;
		
		case UPM_SPRITE:
		{
			vec4 tcolor = texture(sprite_texture, sprite.uv);
			// vec3 blend_color = sprite.color.rgb * sprite.blend;
			// float alpha = tcolor.a * sprite.color.a;
			// color = vec4(tcolor.rgb * blend_color, alpha);
			color = mix(tcolor, tcolor * sprite.color, sprite.blend);
			color.a = in_circle(color.a);
		}
			break;

		case UPM_GLYPH:
		{
			vec4 level = texture(font_texture, glyph.uv);
			float dist = 1.0f - level.r;

			const float twidth = glyph.widths.x;
			const float bwidth = glyph.widths.y;
			const float tedge = glyph.edges.x;
			const float bedge = glyph.edges.y;

			float alpha = 1.0f - smoothstep(twidth, twidth + tedge, dist);
			vec4 text_color = glyph.text_color;// * alpha;

			float border_alpha = 1.0f - smoothstep(bwidth, bwidth + bedge, dist);
			vec4 border_color = glyph.outline_color;// * border_alpha;

			vec4 final_color = mix(border_color, text_color, alpha);

			color = vec4(final_color.rgb, border_alpha);
		}
			break;
	}
}