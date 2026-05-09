#include <fstream>
#include <iostream>
#include <glad/gl.h>
#include <sstream>
#include "shaders.h"

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

    const char* vertexShaderCode = vertexShaderCodeString.c_str();
    const char* fragmentShaderCode = fragmentShaderCodeString.c_str();

    unsigned vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertex);
    glAttachShader(shaderID, fragment);
    glLinkProgram(shaderID);

    glUseProgram(shaderID);

    unsigned int cameraTransformsIndex = glGetUniformBlockIndex(shaderID, "cameraTransformations");
    glUniformBlockBinding(shaderID, cameraTransformsIndex, 0);

    unsigned int materialIndex = glGetUniformBlockIndex(shaderID, "material");
    glUniformBlockBinding(shaderID, materialIndex, 1);

    glUniform1i(glGetUniformLocation(shaderID, "diffuseTexture"), 0);

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
}