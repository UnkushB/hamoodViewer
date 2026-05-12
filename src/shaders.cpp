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

