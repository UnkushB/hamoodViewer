#version 330 core

out vec4 fragColor;

in vec2 texCoord;

layout(std140) uniform material {
   vec3 diffuse;
   float opacity;
   int hasDiffuse;
};

uniform sampler2D diffuseTexture;

void main(){
    //fragColor = vec4(0.2f, 0.5f, 0.3f, 1.0f);
    vec3 diffuseColor = diffuse;
    if(hasDiffuse != -1)
        diffuseColor = texture(diffuseTexture, texCoord).rgb;
    fragColor = vec4(diffuseColor, opacity);
}