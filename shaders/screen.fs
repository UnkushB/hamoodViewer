#version 330 core

// shader inputs
in vec2 texture_coords;

// shader outputs
layout (location = 0) out vec4 frag;

// screen image
uniform sampler2D screen;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 0.1 * 100.0) / (100.0 + 0.1 - z * (100.0 - 0.1));
}

void main()
{
	vec3 hdrColor = (texture(screen, texture_coords).rgb);

	vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	 frag = vec4(mapped, 1.0);

	// float depthValue = texture(screen, texture_coords).r;
    //frag = vec4(vec3(LinearizeDepth(depthValue) / 100.0), 1.0);

	 //frag = vec4((texture(screen, texture_coords).rgb), 1.0);
}