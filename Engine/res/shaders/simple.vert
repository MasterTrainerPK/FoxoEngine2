#version 400 core

layout (location = 0) in vec3 vert_Position;

uniform mat4 u_Proj;

void main(void)
{
	gl_Position = u_Proj * vec4(vert_Position.xy, vert_Position.z - 2.0, 1.0);
}