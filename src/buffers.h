#pragma once
#include <glad/gl.h>
#include "model.h"

struct cameraTransformations {
    glm::vec4 camPos;
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

    unsigned int radianceTexture = 0, envCubeMap = 0, irradianceMap = 0;
    void loadRadianceTexture();
    void createEnvCubeMap(unsigned int cubemapID, unsigned int convoluteID);

    std::vector<unsigned int> diffuseTextures;
    void createDiffuseTextures(std::unordered_map<std::string, std::vector<int>>& texturePaths, std::vector<hamoodMaterial>& materials);

    unsigned int opaqueFBO = 0, transparentFBO = 0;
    void createFrameBuffers();

    GLenum transparentDrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //std::vector<GLenum> transparentDrawBuffers;
    glm::vec4 zeroFillerVec;
    glm::vec4 oneFillerVec;

    unsigned int opaqueTexture = 0, depthTexture = 0, accumTexture = 0, revealTexture = 0;
    void createFrameBufferTextures(int width, int height);

    unsigned int quadVAO = 0, quadVBO = 0;
    void createQuadVAO();

    unsigned int cubeVAO = 0, cubeVBO = 0;
    void createCubeVAO();
};