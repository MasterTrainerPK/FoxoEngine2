#version 400 core

layout (location = 0) out vec4 out_Color;

uniform vec3 u_Color;

void main(void)
{
	out_Color = vec4(u_Color, 1.0);
}