#pragma once
#include "glfwStuff.h"
#include "model.h"
#include "buffers.h"
#include "orbitcam.h"
#include "shaders.h"

class hamoodViewer {
public:
    hamoodWindow myWindow;
    hamoodModel model;
    hamoodBuffers buffers;
    orbit_cam cam;
    hamoodShaders shaders;
    glm::mat4 modelTransform;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    void run();

    void mainLoop();

    void draw();

    void shadowPass(cameraTransformations& lightMatrix);

    void opaquePass(cameraTransformations& camMatrixs, glm::mat4& lightProjView);

    void transparentPass(cameraTransformations& camMatrixs, glm::mat4& lightProjView);

    void compositePass();
};