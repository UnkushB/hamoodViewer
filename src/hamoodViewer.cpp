#include <glad/gl.h>
#include "hamoodViewer.h"
#include <iostream>


void hamoodViewer::run() {
    myWindow.initGLFW();
    model.loadModel("balls");
    buffers.createVertexBuffer(model.vertices);
    buffers.createIndexBuffer(model.indices);
    buffers.createTransformationsUBO();
    shaders.createShaderProgram();
    cam.createCam(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 5.0f, 0.0f, 0.0f);
    unsigned int cameraTransformsIndex = glGetUniformBlockIndex(shaders.shaderID, "cameraTransformations");
    glUniformBlockBinding(shaders.shaderID, cameraTransformsIndex, 0);

    if (cameraTransformsIndex == GL_INVALID_INDEX) {
        std::cout << "cameraTransformations block not found\n";
    }
    glEnable(GL_DEPTH_TEST);
    mainLoop();
}


void hamoodViewer::mainLoop() {
    while (!glfwWindowShouldClose(myWindow.window)) {
        draw();
    }

    glfwTerminate();
}

void hamoodViewer::draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaders.shaderID);

    glm::mat4 modelTransform(1.0f);
    modelTransform = glm::scale(modelTransform, glm::vec3(2.0f / model.radius));
    modelTransform = glm::translate(modelTransform, -model.centroid);
    cam.rotate_x(myWindow.yaw);
    cam.rotate_y(myWindow.pitch);
    glm::mat4 viewMatrix = cam.get_view_matrix();
    glm::mat4 projMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(myWindow.windowWidth) / static_cast<float>(myWindow.windowHeight), 0.1f, 100.0f);
    projMatrix[1][1] *= -1.0f;

    cameraTransformations camMatrixs;
    camMatrixs.model = modelTransform;
    camMatrixs.view = viewMatrix;
    camMatrixs.projections = projMatrix;

    myWindow.yaw = 0.0f;
    myWindow.pitch = 0.0f;

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers.transformationsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, buffers.transformationsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cameraTransformations), &camMatrixs);


    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }

    glfwSwapBuffers(myWindow.window);
    glfwPollEvents();
}