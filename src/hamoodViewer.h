#include <GLFW/glfw3.h>

class hamoodViewer {
public:
    GLFWwindow* window = nullptr;
    int windowWidth = 1000; int windowHeight = 800;
    void initGLFW();

    void mainLoop();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};