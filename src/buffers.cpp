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

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void hamoodBuffers::createIndexBuffer(std::vector<uint32_t>& indices) {
    //if (!glIsBuffer(EBO))
    if (EBO == 0)
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glBufferData(GL_UNIFORM_BUFFER, sizeof(material), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void hamoodBuffers::updateMaterialUBO(material& mat) {
    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(material), &mat);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void hamoodBuffers::createDiffuseTextures(std::unordered_map<std::string, std::vector<int>>& textureNames, std::vector<material>& materials) {
    stbi_set_flip_vertically_on_load(true);
    if (diffuesTextures.size() > 0) {
        glDeleteTextures(diffuesTextures.size(), diffuesTextures.data());
        diffuesTextures.clear();
    }


    int textureIndex = 0;
    for (auto& x : textureNames) {
        std::string texturePath = x.first;
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            /*for (auto& matIndex : x.second) {
                materials[matIndex].diffuseTextureIndex = -1;
            }
            std::cout << "failed to load textuer\n";*/
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        for (auto& matIndex : x.second) {
            materials[matIndex].diffuse[3] = textureIndex;
            std::cout << "loading texture into material: " << materials[matIndex].diffuse[3] << std::endl;
        }
        diffuesTextures.push_back(texture);
        textureIndex++;
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}