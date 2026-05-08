#include "buffers.h"
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

void hamoodBuffers::createVertexBuffer(std::vector<vertex>& vertices) {
    //if (!glIsVertexArray(VAO)) {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, pos)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, uv)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void hamoodBuffers::createIndexBuffer(std::vector<uint32_t>& indices) {
    //if (!glIsBuffer(EBO))
    if (EBO == 0)
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void hamoodBuffers::createCameraUBO() {
    //if (!glIsBuffer(cameraUBO))
    if (cameraUBO == 0)
        glGenBuffers(1, &cameraUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraTransformations), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void hamoodBuffers::updateCameraUBO(cameraTransformations& camTransforms) {
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cameraTransformations), &camTransforms);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void hamoodBuffers::createMaterialUBO() {
    if (materialUBO == 0)
        glGenBuffers(1, &materialUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(hamoodMaterial), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void hamoodBuffers::updateMaterialUBO(hamoodMaterial& material) {
    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(hamoodMaterial), &material);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}