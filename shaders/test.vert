#version 430 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout(std140) uniform cameraTransformations {
    mat4 model;
    mat4 view;
    mat4 projection;
};
out vec2 texCoord;

uniform mat4 localTransform;

void main(){
    gl_Position =  projection * view * model * localTransform * vec4(pos, 1.0f);
    texCoord = uv;
}