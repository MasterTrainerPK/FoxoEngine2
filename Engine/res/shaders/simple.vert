#version 400 core

layout (location = 0) in vec3 vert_Position;
layout (location = 1) in vec3 vert_Normal;
layout (location = 2) in vec2 vert_TexCoord;

out vec3 frag_Normal;

uniform mat4 u_Model;
uniform mat4 u_Proj;

void main(void)
{
	gl_Position = u_Proj * u_Model * vec4(vert_Position, 1.0);
	frag_Normal = transpose(inverse(mat3(u_Model))) * vert_Normal;
}