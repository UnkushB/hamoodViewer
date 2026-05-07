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

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}