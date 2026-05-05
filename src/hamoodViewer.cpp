#include "hamoodViewer.h"
#include <iostream>


void hamoodViewer::run() {
    myWindow.initGLFW();

    mainLoop();
}


void hamoodViewer::mainLoop() {
    while (!glfwWindowShouldClose(myWindow.window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(myWindow.window);
        glfwPollEvents();
    }

    glfwTerminate();
}