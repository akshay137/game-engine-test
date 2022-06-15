#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

layout (location = 2) in vec4 text_color;
layout (location = 3) in vec4 outline_color;

layout (location = 4) in float width;
layout (location = 4, component = 1) in float edge;
layout (location = 4, component = 2) in float border_width;
layout (location = 4, component = 3) in float border_edge;

layout (std140, binding = 0) uniform rstate
{
	layout (offset = 0) mat4 orthographic;
	layout (offset = 64) vec4 viewport;
};

out VS_OUT
{
	vec2 uv;
	vec4 text_color;
	vec4 outline_color;
	vec2 widths;
	vec2 edges;
} vs;

void main()
{
	vec4 pos4 = vec4(position, 0.0f, 1.0f);
	gl_Position = orthographic * pos4;

	vs.uv = uv;
	vs.text_color = text_color;
	vs.outline_color = outline_color;
	vs.widths = vec2(width, border_width);
	vs.edges = vec2(edge, border_edge);
}