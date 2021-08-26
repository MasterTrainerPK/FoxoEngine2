#version 400 core

in vec3 frag_Normal;
in vec2 frag_TexCoord;

layout (location = 0) out vec4 out_Color;

uniform vec3 u_Color;

void main(void)
{
	float brightness = max(dot(vec3(0.0, 0.0, 1.0), normalize(frag_Normal)), 0.2);
	out_Color = vec4(vec3(frag_TexCoord.xy * 2.0 - 1.0, 0.0) * u_Color * brightness, 1.0);
}