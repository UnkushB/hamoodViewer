#pragma once
#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>

struct vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

struct hamoodMesh {
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;
    uint32_t vertexCount;
    glm::mat4 localTransform;
    int materialIndex;
};

struct alignas(16) hamoodMaterial {
    glm::vec3 diffuse;
    float opacity;
    int diffuseTextureIndex = -1;
    int diffuseHasOpacity = -1;
    int padding[2];
};

class hamoodModel {
public:
    std::string defaultModel = "P:/learnOpenGL/models/despacito/DespacitoSpider.obj";
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<hamoodMesh> meshes;
    std::vector<hamoodMaterial> materials;
    std::unordered_map<std::string, std::vector<int>> diffuseTexturePaths;
    glm::vec3 centroid;
    float radius;
    void loadModel(const std::string& modelFilePath);

    void processNode(aiNode* node, const aiScene* scene, glm::mat4 accumTransforms);
};