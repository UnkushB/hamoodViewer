#version 430 core

// shader inputs
in vec2 texture_coords;

// shader outputs
layout (location = 0) out vec4 frag;

// screen image
uniform sampler2D msdfAtlas;

uniform int clicked;

float median(vec3 rgb) {
  return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}

void main()
{ 
    if(texture_coords.x < 2.0){
      float texel = median(texture(msdfAtlas, texture_coords).rgb);
      float w = fwidth(texel);
      float opacity = smoothstep(0.5 - w, 0.5 + w, texel);
      if(clicked == 0)
      frag = vec4(vec3(0.0), opacity);
      else
        frag = vec4(vec3(1.0), opacity);
    }
    else{
      frag = vec4(0.0, 0.0, 0.0, 1.0);
    }
	// frag = vec4(texture(msdfAtlas, texture_coords).rgb, 1.0f);
}