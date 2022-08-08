#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec2 uv;

const vec3 normal = vec3(0, 0, 1); // always forward facing in local space

out VS_OUT
{
	vec3 world_pos;
	vec2 tex_coords;
	vec2 uv;
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

// TODO: make it part of vertex attributes
layout (location = 0) uniform mat4 model;

void main()
{
	vec4 mpos = model * vec4(position, 1);
	vec4 final_pos = projection * view * mpos;
	gl_Position = final_pos;

	vs.world_pos = mpos.xyz;
	vs.tex_coords = tex_coords;
	vs.uv = uv;
}