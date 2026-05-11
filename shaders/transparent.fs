#version 430 core

in vec2 texCoord;

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

layout(std140) uniform material {
   vec3 diffuse;
   float opacity;
   int hasDiffuse;
   int diffuseHasOpacity;
};

uniform sampler2D diffuseTexture;

void main(){
    //fragColor = vec4(0.2f, 0.5f, 0.3f, 1.0f);
    vec3 diffuseColor = diffuse;
    float finalOpacity = opacity;
    if(hasDiffuse != -1)
        diffuseColor = texture(diffuseTexture, texCoord).rgb;
    if(diffuseHasOpacity >= 0)
        finalOpacity = texture(diffuseTexture, texCoord).a;

    if(finalOpacity >= 0.95)
        discard;

    float weight = clamp(pow(min(1.0, finalOpacity * 10.0) + 0.01, 3.0) * 1e2 * 
                         pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    accum = vec4(diffuseColor * finalOpacity, finalOpacity) * weight;
   //accum = vec4(0.3, 0.6, 0.2, finalOpacity);

    // store pixel revealage threshold
    reveal = finalOpacity;

    //accum = vec4(1,0,0,1);
    //reveal = 1.0;
}