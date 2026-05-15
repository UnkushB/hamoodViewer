#pragma once
#include <vector>
#include <string>

class hamoodShaders {
public:
    unsigned int shaderID, quadID, transparentID, compositeID, cubemapID, skyboxID, convolutionID;
    unsigned int localTransformLocation;
    std::string readShaderCode(std::string&& shaderPath);
    void createShaderProgram();
};