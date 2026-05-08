#pragma once
#include "model.h"

struct cameraTransformations {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projections;
};

class hamoodBuffers {
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0, cameraUBO = 0, materialUBO = 0;
    void createVertexBuffer(std::vector<vertex>& vertices);

    void createIndexBuffer(std::vector<uint32_t>& indices);

    void createCameraUBO();
    void updateCameraUBO(cameraTransformations& camTransforms);

    void createMaterialUBO();
    void updateMaterialUBO(hamoodMaterial& material);
};