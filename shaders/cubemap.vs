#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

layout(std140) uniform cameraTransformations {
    vec4 camPos;
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main()
{
    localPos = aPos;  
    gl_Position =  projection * view * vec4(localPos, 1.0);
}