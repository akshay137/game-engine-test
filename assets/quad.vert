#version 450 core

out VS_OUT
{
	vec3 color;
} vs;

void main()
{
	vec2 pos[3] = { vec2(0.0f, 0.5f), vec2(0.5f, -0.5f), vec2(-0.5f, -0.5f) };
	vec3 colors[3] = {
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f)
	};

	gl_Position = vec4(pos[gl_VertexID], 0.0f, 1.0f);
	vs.color = colors[gl_VertexID];
}