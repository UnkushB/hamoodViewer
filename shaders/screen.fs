#version 330 core

// shader inputs
in vec2 texture_coords;

// shader outputs
layout (location = 0) out vec4 frag;

// screen image
uniform sampler2D screen;

void main()
{
	vec3 hdrColor = (texture(screen, texture_coords).rgb);

	vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	 frag = vec4(mapped, 1.0);

	 //frag = vec4((texture(screen, texture_coords).rgb), 1.0);
}