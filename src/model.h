#pragma once
#include <string>
#include <glm/glm.hpp>

struct vertex {
    glm::vec3 pos;
};

struct mesh {
    uint32_t indexOffset;
    uint32_t indexCount;
    int materialIndex;
};

class hamoodModel {
public:
    std::string defaultModel = "P:/learnOpenGL/models/despacito/DespacitoSpider.obj";
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<mesh> meshes;
    glm::vec3 centroid;
    float radius;
    void loadModel(const std::string& modelFilePath);


};