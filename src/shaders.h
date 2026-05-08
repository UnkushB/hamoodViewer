#pragma once
#include <vector>
#include <string>

class hamoodShaders {
public:
    unsigned int shaderID;
    unsigned int localTransformLocation;
    std::string readShaderCode(std::string&& shaderPath);
    void createShaderProgram();
};