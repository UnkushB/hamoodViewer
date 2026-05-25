#version 430 core

in vec2 texture_coords;

layout (location = 0) out vec4 frag;

uniform sampler2D screen;

void main()
{
	vec3 hdrColor = (texture(screen, texture_coords).rgb);

	vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	 frag = vec4(mapped, 1.0);
}