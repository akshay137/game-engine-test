#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 normal;

out VS
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

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform float factor;

void main()
{
	vec4 pivot = model * vec4(normal, 1);
	vec4 world_pos = model * vec4(position, 1);
	
	world_pos = pivot + (world_pos - pivot) * factor;

	gl_Position = projection * view * world_pos;

	vs.world_position = world_pos.xyz;
	vs.uv = uv;
	vs.color = color;
	vs.normal = normal;
}