#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 color;

out VS_OUT
{
	vec2 uv;
	vec3 color;
} vs;

void main()
{
	gl_Position = vec4(position, 0.0f, 1.0f);

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