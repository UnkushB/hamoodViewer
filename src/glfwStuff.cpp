#include <glad/gl.h>
#include "glfwStuff.h"
#include <iostream>
#include <glm/glm.hpp>

void hamoodWindow::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(windowWidth, windowHeight, "hamoodViewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "failed to create window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetScrollCallback(window, scrollCallback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cout << "Failed to initialize OpenGL context\n";
        return;
    }

    windowsFile.lpstrFilter = "";
    windowsFile.lpstrTitle = "Select Obj File";
    windowsFile.nMaxFile = sizeof(buff);
    windowsFile.lpstrFile = buff;
}

void hamoodWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<hamoodWindow*>(glfwGetWindowUserPointer(window));
    app->windowWidth = width;
    app->windowHeight = height;
    glViewport(0, 0, width, height);
}

void hamoodWindow::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto app = reinterpret_cast<hamoodWindow*>(glfwGetWindowUserPointer(window));
    if (!app->leftButtonDown)
        return;

    app->yaw += (xpos - app->lastX) * 0.05;
    app->pitch += (ypos - app->lastY) * 0.05;

    app->yaw = std::fmod(app->yaw, 360.0f);
    app->pitch = std::fmod(app->pitch, 360.0f);

    app->lastX = xpos;
    app->lastY = ypos;
}

void hamoodWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto app = reinterpret_cast<hamoodWindow*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            app->leftButtonDown = true;
            glfwGetCursorPos(window, &app->lastX, &app->lastY);
        }
        else if (action == GLFW_RELEASE)
            app->leftButtonDown = false;
    }
}

void hamoodWindow::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<hamoodWindow*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        WINBOOL result = GetOpenFileNameA(&app->windowsFile);
        if (result) {
            app->reloadModel = true;
        }
    }
}

void hamoodWindow::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = reinterpret_cast<hamoodWindow*>(glfwGetWindowUserPointer(window));
    app->scrollOffset = yoffset;
}