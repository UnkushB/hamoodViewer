#include <fstream>
#include <iostream>
#include <glad/gl.h>
#include <sstream>
#include "shaders.h"

void checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

std::string hamoodShaders::readShaderCode(std::string&& shaderPath) {

    std::ifstream shaderFile(shaderPath);

    if (!shaderFile.is_open()) {
        std::cout << "failed to open shader file: " << shaderPath << "\n";
        return {};
    }

    std::stringstream shaderStream;

    shaderStream << shaderFile.rdbuf();

    shaderFile.close();

    return shaderStream.str();
}

void hamoodShaders::createShaderProgram() {
    std::string vertexShaderCodeString = readShaderCode("shaders/test.vert");
    std::string fragmentShaderCodeString = readShaderCode("shaders/test.frag");
    std::string quadVertexShaderString = readShaderCode("shaders/screen.vs");
    std::string quadFragmentShaderString = readShaderCode("shaders/screen.fs");
    std::string transparentVertexString = readShaderCode("shaders/transparent.vs");
    std::string transparentFragmentString = readShaderCode("shaders/transparent.fs");
    std::string compositeVertexString = readShaderCode("shaders/composite.vs");
    std::string compositeFragmentString = readShaderCode("shaders/composite.fs");
    std::string cubemapVertexString = readShaderCode("shaders/cubemap.vs");
    std::string cubemapFragmentString = readShaderCode("shaders/cubemap.fs");
    std::string skyBoxVertexString = readShaderCode("shaders/skybox.vs");
    std::string skyBoxFragmentString = readShaderCode("shaders/skybox.fs");
    std::string convolutionFragmentString = readShaderCode("shaders/convolute.fs");
    std::string prefilterFragmentString = readShaderCode("shaders/prefilter.fs");
    std::string brdfShaderFragmentString = readShaderCode("shaders/brdfShader.fs");
    std::string shadowMapVertexString = readShaderCode("shaders/shadowMap.vs");
    std::string shadowMapFragmentString = readShaderCode("shaders/shadowMap.fs");

    const char* vertexShaderCode = vertexShaderCodeString.c_str();
    const char* fragmentShaderCode = fragmentShaderCodeString.c_str();

    unsigned vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "opaqueVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "opaqueFS");

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertex);
    glAttachShader(shaderID, fragment);
    glLinkProgram(shaderID);
    checkCompileErrors(shaderID, "opaque");

    glUseProgram(shaderID);

    unsigned int cameraTransformsIndex = glGetUniformBlockIndex(shaderID, "cameraTransformations");
    glUniformBlockBinding(shaderID, cameraTransformsIndex, 0);

    unsigned int materialIndex = glGetUniformBlockIndex(shaderID, "material");
    glUniformBlockBinding(shaderID, materialIndex, 1);

    glUniform1i(glGetUniformLocation(shaderID, "diffuseTexture"), 0);

    glUniform1i(glGetUniformLocation(shaderID, "irradianceMap"), 1);

    glUniform1i(glGetUniformLocation(shaderID, "brdfLUT"), 2);

    glUniform1i(glGetUniformLocation(shaderID, "prefilterMap"), 3);

    glUniform1i(glGetUniformLocation(shaderID, "shadowMap"), 4);

    glUniform1i(glGetUniformLocation(shaderID, "metallicTexture"), 5);

    glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 6);

    glUniform1i(glGetUniformLocation(shaderID, "aoMap"), 7);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* cubemapVertexCode = cubemapVertexString.c_str();
    const char* cubemapFragmentCode = cubemapFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &cubemapVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "cubemapVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &cubemapFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "cubemapFS");

    cubemapID = glCreateProgram();
    glAttachShader(cubemapID, vertex);
    glAttachShader(cubemapID, fragment);
    glLinkProgram(cubemapID);
    checkCompileErrors(cubemapID, "cubemap");

    glUseProgram(cubemapID);

    cameraTransformsIndex = glGetUniformBlockIndex(cubemapID, "cameraTransformations");
    glUniformBlockBinding(cubemapID, cameraTransformsIndex, 0);

    glUniform1i(glGetUniformLocation(cubemapID, "equirectangularMap"), 0);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* transparentVertexCode = transparentVertexString.c_str();
    const char* transparentFragmentCode = transparentFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &transparentVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "transparentVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &transparentFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "transparentFS");

    transparentID = glCreateProgram();
    glAttachShader(transparentID, vertex);
    glAttachShader(transparentID, fragment);
    glLinkProgram(transparentID);
    checkCompileErrors(transparentID, "transparent");

    glUseProgram(transparentID);

    cameraTransformsIndex = glGetUniformBlockIndex(transparentID, "cameraTransformations");
    glUniformBlockBinding(transparentID, cameraTransformsIndex, 0);

    materialIndex = glGetUniformBlockIndex(transparentID, "material");
    glUniformBlockBinding(transparentID, materialIndex, 1);

    glUniform1i(glGetUniformLocation(transparentID, "diffuseTexture"), 0);

    glUniform1i(glGetUniformLocation(transparentID, "irradianceMap"), 1);

    glUniform1i(glGetUniformLocation(transparentID, "brdfLUT"), 2);

    glUniform1i(glGetUniformLocation(transparentID, "prefilterMap"), 3);

    glUniform1i(glGetUniformLocation(transparentID, "shadowMap"), 4);

    glUniform1i(glGetUniformLocation(transparentID, "metallicTexture"), 5);

    glUniform1i(glGetUniformLocation(transparentID, "normalMap"), 6);

    glUniform1i(glGetUniformLocation(transparentID, "aoMap"), 7);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* compositeVertexCode = compositeVertexString.c_str();
    const char* compositeFragmentCode = compositeFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &compositeVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "compositeVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &compositeFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "compositeFS");

    compositeID = glCreateProgram();
    glAttachShader(compositeID, vertex);
    glAttachShader(compositeID, fragment);
    glLinkProgram(compositeID);
    checkCompileErrors(compositeID, "composite");

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* skyBoxVertexCode = skyBoxVertexString.c_str();
    const char* skyBoxFragmentCode = skyBoxFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &skyBoxVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "skyboxVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &skyBoxFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "skyboxFS");

    skyboxID = glCreateProgram();
    glAttachShader(skyboxID, vertex);
    glAttachShader(skyboxID, fragment);
    glLinkProgram(skyboxID);
    checkCompileErrors(skyboxID, "skybox");

    glUseProgram(skyboxID);

    cameraTransformsIndex = glGetUniformBlockIndex(skyboxID, "cameraTransformations");
    glUniformBlockBinding(skyboxID, cameraTransformsIndex, 0);

    glUniform1i(glGetUniformLocation(skyboxID, "environmentMap"), 0);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* convolutionFragmentCode = convolutionFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &cubemapVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "cubemapVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &convolutionFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "convolutionFS");

    convolutionID = glCreateProgram();
    glAttachShader(convolutionID, vertex);
    glAttachShader(convolutionID, fragment);
    glLinkProgram(convolutionID);
    checkCompileErrors(convolutionID, "convolution");

    glUseProgram(convolutionID);

    cameraTransformsIndex = glGetUniformBlockIndex(convolutionID, "cameraTransformations");
    glUniformBlockBinding(convolutionID, cameraTransformsIndex, 0);

    glUniform1i(glGetUniformLocation(convolutionID, "environmentMap"), 0);

    glUseProgram(0);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* prefilterFragmentCode = prefilterFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &cubemapVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "cubemapVS");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &prefilterFragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "prefilterFS");

    prefilterID = glCreateProgram();
    glAttachShader(prefilterID, vertex);
    glAttachShader(prefilterID, fragment);
    glLinkProgram(prefilterID);
    checkCompileErrors(prefilterID, "prefilter");

    glUseProgram(prefilterID);

    cameraTransformsIndex = glGetUniformBlockIndex(prefilterID, "cameraTransformations");
    glUniformBlockBinding(prefilterID, cameraTransformsIndex, 0);

    glUniform1i(glGetUniformLocation(prefilterID, "environmentMap"), 0);

    glUniform1f(glGetUniformLocation(prefilterID, "roughness"), 1.0f);

    glUseProgram(0);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* quadVertexShaderCode = quadVertexShaderString.c_str();
    const char* quadFragmentShaderCode = quadFragmentShaderString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &quadVertexShaderCode, nullptr);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &quadFragmentShaderCode, nullptr);
    glCompileShader(fragment);

    quadID = glCreateProgram();
    glAttachShader(quadID, vertex);
    glAttachShader(quadID, fragment);
    glLinkProgram(quadID);

    glUseProgram(quadID);

    glUniform1i(glGetUniformLocation(quadID, "screen"), 0);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* brdfShaderFragmentCode = brdfShaderFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &quadVertexShaderCode, nullptr);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &brdfShaderFragmentCode, nullptr);
    glCompileShader(fragment);

    brdfShaderID = glCreateProgram();
    glAttachShader(brdfShaderID, vertex);
    glAttachShader(brdfShaderID, fragment);
    glLinkProgram(brdfShaderID);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    const char* shadowMapVertexCode = shadowMapVertexString.c_str();
    const char* shadowMapFragmentCode = shadowMapFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &shadowMapVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "shadowMapvs");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &shadowMapFragmentCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "shadowMapfs");

    shadowMapID = glCreateProgram();
    glAttachShader(shadowMapID, vertex);
    glAttachShader(shadowMapID, fragment);
    glLinkProgram(shadowMapID);
    checkCompileErrors(shadowMapID, "shadowMap");

    glUseProgram(shadowMapID);

    cameraTransformsIndex = glGetUniformBlockIndex(shadowMapID, "cameraTransformations");
    glUniformBlockBinding(shadowMapID, cameraTransformsIndex, 0);

    materialIndex = glGetUniformBlockIndex(shadowMapID, "material");
    glUniformBlockBinding(shadowMapID, materialIndex, 1);

    glUniform1i(glGetUniformLocation(shadowMapID, "diffuseTexture"), 0);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::string uiVertexString(readShaderCode("shaders/ui.vs"));
    std::string uiFragmentString(readShaderCode("shaders/ui.fs"));

    const char* uiVertexCode = uiVertexString.c_str();
    const char* uiFragmentCode = uiFragmentString.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &uiVertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "uivs");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &uiFragmentCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "uifs");

    uiID = glCreateProgram();
    glAttachShader(uiID, vertex);
    glAttachShader(uiID, fragment);
    glLinkProgram(uiID);
    checkCompileErrors(uiID, "ui");

    glUseProgram(uiID);

    glUniform1i(glGetUniformLocation(uiID, "msdfAtlas"), 0);

    glUseProgram(0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

