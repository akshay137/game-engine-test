#version 450 core

in VS_OUT
{
	vec3 color;
} vs;

layout (location = 0) out vec4 color;

void main()
{
	color = vec4(vs.color, 1.0f);
}