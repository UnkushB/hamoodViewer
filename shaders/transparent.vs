#version 430 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 aNorm;

layout(std140) uniform cameraTransformations {
    vec4 camPos;
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform mat4 localTransform;

out vec2 texCoord;
out vec3 camP;
out vec3 Norm;
out vec3 worldPos;


void main(){
    gl_Position =  projection * view * model * localTransform * vec4(pos, 1.0f);
    texCoord = uv;
    worldPos = vec3(model * localTransform * vec4(pos, 1.0f));
    Norm = normalize(mat3(transpose(inverse(model * localTransform))) * aNorm);
    camP = vec3(camPos);
}