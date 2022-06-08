#version 450 core

in VS_OUT
{
	vec2 uv;
	vec3 color;
} vs;

layout (binding = 0) uniform sampler2D diffuse;

layout (location = 0) out vec4 color;

void main()
{
	vec4 tcolor = texture(diffuse, vs.uv);
	// color = vec4(vs.color, 1.0f);
	color = tcolor;
}