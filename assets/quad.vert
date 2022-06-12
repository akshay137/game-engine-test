#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 color;

layout (std140, binding = 0) uniform rstate
{
	layout (offset = 0) mat4 orthographic;
	layout (offset = 64) vec4 viewport;
};

out VS_OUT
{
	vec3 wpos;
	vec2 uv;
	vec3 color;
} vs;

void main()
{
	const vec4 pos4 = vec4(position * 64 + vec2(100, 100), 0.0f, 1.0f);
	vec4 pos = orthographic * pos4;
	gl_Position = pos;

	vs.wpos = pos4.xyz;
	vs.uv = uv;
	vs.color = color;

	// static program
	// vec2 pos[3] = { vec2(0.0f, 0.5f), vec2(0.5f, -0.5f), vec2(-0.5f, -0.5f) };
	// vec3 colors[3] = {
	// 	vec3(1.0f, 0.0f, 0.0f),
	// 	vec3(0.0f, 1.0f, 0.0f),
	// 	vec3(0.0f, 0.0f, 1.0f)
	// };

	// gl_Position = vec4(pos[gl_VertexID], 0.0f, 1.0f);
	// vs.color = colors[gl_VertexID];
}