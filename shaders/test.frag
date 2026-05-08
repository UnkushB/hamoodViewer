#version 330 core

out vec4 fragColor;

in vec2 texCoord;

layout(std140) uniform material {
   vec3 diffuse;
   float opacity;
};

void main(){
    //fragColor = vec4(0.2f, 0.5f, 0.3f, 1.0f);
    fragColor = vec4(diffuse, opacity);
}