#version 430 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout(std140) uniform cameraTransformations {
    vec4 camPos;
    mat4 model;
    mat4 view;
    mat4 projection;
};
out vec2 texCoord;
out vec3 camP;
out vec3 Norm;
out vec3 worldPos;
out vec4 fragPosLightSpace;
out mat3 tbn;

uniform mat4 localTransform;
uniform mat4 lightSpaceMatrix;

void main(){
    texCoord = uv;
    worldPos = vec3(model * localTransform * vec4(pos, 1.0f));
    gl_Position =  projection * view * model * localTransform * vec4(pos, 1.0f);
    Norm = normalize(mat3(transpose(inverse(model * localTransform))) * aNorm);
    camP = vec3(camPos);
    fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 T = normalize(vec3(model * localTransform * vec4(aTangent,   0.0)));
   vec3 B = normalize(vec3(model * localTransform * vec4(aBitangent, 0.0)));
   vec3 N = normalize(vec3(model * localTransform * vec4(aNorm,    0.0)));
   tbn = mat3(T, B, N);
}