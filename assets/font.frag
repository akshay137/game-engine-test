#version 450 core

in VS_OUT
{
	vec2 uv;
	vec4 text_color;
	vec4 outline_color;
	float width;
	float edge;
} vs;

layout (binding = 1) uniform sampler2D atlas;

layout (location = 0) out vec4 color;

// const float width = 0.3f;
// const float edge = 0.7f;

void main()
{
	vec4 level = texture(atlas, vs.uv);
	float dist = 1.0f - level.r;

	float a = 1.0f - smoothstep(vs.width, vs.width + vs.edge, dist);
	vec4 text_color = vs.text_color * a;

	vec4 final_color = text_color;

	color = final_color;
}