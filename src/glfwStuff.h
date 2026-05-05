#include <GLFW/glfw3.h>

class hamoodWindow {
public:
    GLFWwindow* window = nullptr;
    int windowWidth = 1000; int windowHeight = 800;
    void initGLFW();


    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};