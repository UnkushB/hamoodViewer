#pragma once
#include <GLFW/glfw3.h>

class hamoodWindow {
public:
    GLFWwindow* window = nullptr;
    int windowWidth = 1000; int windowHeight = 800;
    void initGLFW();


    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    bool leftButtonDown = false;
    double lastX = 0.0f; double lastY = 0.0f;
    float yaw = 0.0f; float pitch = 0.0f;
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int actions, int mods);
};