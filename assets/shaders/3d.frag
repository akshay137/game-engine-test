#version 450 core

in VS
{
	vec3 world_position;
	vec2 uv;
	vec3 color;
	vec3 normal;
} vs;

struct TimeData
{
	float system;
	float frame;
};

layout (std140, binding = 0) uniform rstate
{
	layout (offset = 0) mat4 projection;
	layout (offset = 64) mat4 view;
	layout (offset = 128) vec4 viewport;
	layout (offset = 144) TimeData time_data;
};

layout (binding = 0) uniform sampler2D diffuse;

layout (location = 0) out vec4 color;

void main()
{
	vec4 texel = texture(diffuse, vs.uv);
	// color = mix(vec4(vs.color, 1), texel, 0.5f);
	color = texel;
}