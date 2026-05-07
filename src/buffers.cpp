#include "buffers.h"
#include <glad/gl.h>

void hamoodBuffers::createVertexBuffer(std::vector<vertex>& vertices) {
    if (!glIsVertexArray(VAO)) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, pos)));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void hamoodBuffers::createIndexBuffer(std::vector<uint32_t>& indices) {
    if (!glIsBuffer(EBO))
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
};

void hamoodBuffers::createTransformationsUBO() {
    if (!glIsBuffer(transformationsUBO))
        glGenBuffers(1, &transformationsUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, transformationsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraTransformations), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformationsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}