#include <glad/gl.h>
#include "hamoodViewer.h"
#include <iostream>


void hamoodViewer::run() {
    myWindow.initGLFW();
    model.loadModel(model.defaultModel);
    buffers.createVertexBuffer(model.vertices);
    buffers.createIndexBuffer(model.indices);
    buffers.createCameraUBO();
    buffers.createMaterialUBO();
    buffers.createDiffuseTextures(model.diffuseTextureNames, model.materials);
    shaders.createShaderProgram();
    cam.createCam(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 5.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    mainLoop();
}


void hamoodViewer::mainLoop() {
    while (!glfwWindowShouldClose(myWindow.window)) {
        glfwPollEvents();
        if (myWindow.reloadModel) {
            model.loadModel(myWindow.windowsFile.lpstrFile);
            buffers.createVertexBuffer(model.vertices);
            buffers.createIndexBuffer(model.indices);
            std::cout << "starting diffuse textures creation\n";
            buffers.createDiffuseTextures(model.diffuseTextureNames, model.materials);
            std::cout << "made diffuse textures\n";
            std::cout << model.diffuseTextureNames.size() << std::endl;
            myWindow.reloadModel = false;
        }
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

    buffers.updateCameraUBO(camMatrixs);


    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        buffers.updateMaterialUBO(model.materials[mesh.materialIndex]);
        glActiveTexture(GL_TEXTURE0);
        if (model.materials[mesh.materialIndex].diffuse[3] != -1) {
            glBindTexture(GL_TEXTURE_2D, buffers.diffuesTextures[model.materials[mesh.materialIndex].diffuse[3]]);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }

    glfwSwapBuffers(myWindow.window);

}