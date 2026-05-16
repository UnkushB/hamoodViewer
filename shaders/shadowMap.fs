#version 430 core

layout(std140) uniform material {
   vec3 diffuse;
   float opacity;
   float Metallic;
   float Roughness;
   int hasDiffuse;
   int diffuseHasOpacity;
};

uniform sampler2D diffuseTexture;

in vec2 texCoord;

void main()
{             
    float finalOpacity = opacity;
    if(diffuseHasOpacity >= 0)
        finalOpacity = texture(diffuseTexture, texCoord).a;

    if(finalOpacity < 0.95)
       discard;

    gl_FragDepth = gl_FragCoord.z;
} 