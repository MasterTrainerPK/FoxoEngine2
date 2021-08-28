#version 400 core

in vec3 frag_Normal;
in vec2 frag_TexCoord;

layout (location = 0) out vec4 out_Color;

uniform sampler2D u_Albedo;

void main(void)
{
	vec4 texColor = texture(u_Albedo, frag_TexCoord);

	float brightness = max(dot(vec3(0.0, 0.0, 1.0), normalize(frag_Normal)), 0.2);
	out_Color = vec4(texColor.rgb * brightness, texColor.a);
}