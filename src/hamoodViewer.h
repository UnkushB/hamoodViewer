#pragma once
#include "glfwStuff.h"
#include "model.h"
#include "buffers.h"
#include "orbitcam.h"
#include "shaders.h"

class hamoodViewer {
public:
    hamoodWindow myWindow{};
    hamoodModel model{};
    hamoodBuffers buffers{};
    orbit_cam cam{};
    hamoodShaders shaders{};
    void run();

    void mainLoop();

    void draw();
};