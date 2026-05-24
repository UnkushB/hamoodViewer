#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <windows.h>

class hamoodWindow {
public:
    GLFWwindow* window = nullptr;
    int windowWidth = 1000; int windowHeight = 800;
    void initGLFW();
    bool resized = false;
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    bool leftButtonDown = false;
    double lastX = 0.0f; double lastY = 0.0f;
    float yaw = 0.0f; float pitch = 0.0f;
    int clicked = 0;
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int actions, int mods);

    bool reloadModel = false;
    OPENFILENAMEA windowsFile = { sizeof(OPENFILENAMEA) };
    char buff[MAX_PATH] = {};
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    float scrollOffset = 0.0f;
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};