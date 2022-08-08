#version 450 core

in VS_OUT
{
	vec3 world_pos;
	vec2 tex_coords;
	vec2 uv;
} vs;

layout (location = 0) out vec4 color;

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

layout (binding = 0) uniform sampler2D tex0;

layout (location = 1) uniform float start_angle;
layout (location = 2) uniform float pie_angle;
layout (location = 3) uniform mat4 uv_tranform;
layout (location = 4) uniform float ratio;

void main()
{
	vec2 uv = (uv_tranform * vec4(vs.uv, 0, 1)).xy;
	vec4 pixel = texture(tex0, uv);

	uv = vs.uv * vec2(ratio, 0.5);
	vec2 center = vec2(0.5) * vec2(ratio, 1);

	float d = 0.5 - distance(uv, center);
	float inc = step(0, d);
	// 120
	float hang = pie_angle * 0.5f; // 60
	float lower = 1.57 - hang; // 30
	float upper = 1.57 + hang; // 120
	float inp = inc;

	uv = uv - vec2(0.5f) * vec2(ratio, 1);
	float angle = 3.14 + atan(uv.y, uv.x); // 0 to 360
	if (angle < lower) inp = 0;
	if (angle > upper) inp = 0;

	color = vec4(pixel.rgb, pixel.a * inp);
}