#pragma once
#include "model.h"

struct cameraTransformations {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projections;
};

class hamoodBuffers {
public:
    unsigned int VAO, VBO, EBO, transformationsUBO = 0;
    void createVertexBuffer(std::vector<vertex>& vertices);
    void createIndexBuffer(std::vector<uint32_t>& indices);
    void createTransformationsUBO();
};