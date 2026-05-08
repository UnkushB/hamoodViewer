#version 330 core

out vec4 fragColor;

layout(std140) uniform material {
    vec4 diffuse;
};

uniform sampler2D diffuseTexture;

in vec2 texCoord;

void main(){
    //fragColor = vec4(0.2f, 0.5f, 0.3f, 1.0f);
    vec3 diffuseColor = diffuse.rgb;
    if(diffuse.a != -1){
        diffuseColor = texture(diffuseTexture, texCoord).rgb;
    }
    fragColor = vec4(diffuseColor, 1.0f);
}