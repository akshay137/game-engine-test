#version 450 core

in VS_OUT
{
	vec2 uv;
	vec4 text_color;
	vec4 outline_color;
	vec2 widths;
	vec2 edges;
} vs;

layout (binding = 1) uniform sampler2D atlas;

layout (location = 0) out vec4 color;

void main()
{
	vec4 level = texture(atlas, vs.uv);
	float dist = 1.0f - level.r;

	const float twidth = vs.widths.x;
	const float bwidth = vs.widths.y;
	const float tedge = vs.edges.x;
	const float bedge = vs.edges.y;

	float a = 1.0f - smoothstep(twidth, twidth + tedge, dist);
	vec4 text_color = vs.text_color * a;

	float ba = 1.0f - smoothstep(bwidth, bwidth + bedge, dist);
	vec4 border_color = vs.outline_color * ba;

	float final_alpha = a + (1.0f - a) * ba;

	vec4 final_color = mix(vs.outline_color, vs.text_color, a / final_alpha);

	color = vec4(final_color.rgb, final_alpha);
}