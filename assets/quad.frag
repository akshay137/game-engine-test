#version 450 core

in VS_OUT
{
	vec3 wpos;
	vec2 uv;
	float blend;
	vec4 color;
} vs;

layout (binding = 0) uniform sampler2D diffuse;

layout (location = 0) out vec4 color;

void main()
{
	vec4 tcolor = texture(diffuse, vs.uv);
	color = mix(tcolor, vs.color, vs.blend);
	// color = mix(tcolor, vec4(1, 0, 0, 1), 0);
}