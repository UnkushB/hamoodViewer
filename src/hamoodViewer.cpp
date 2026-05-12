#include <glad/gl.h>
#include "hamoodViewer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void hamoodViewer::run() {
    myWindow.initGLFW();
    shaders.createShaderProgram();
    model.loadModel(model.defaultModel);
    buffers.createVertexBuffer(model.vertices);
    buffers.createIndexBuffer(model.indices);
    buffers.createCameraUBO();
    buffers.createMaterialUBO();
    buffers.createDiffuseTextures(model.diffuseTexturePaths, model.materials);
    buffers.createFrameBuffers();
    buffers.createFrameBufferTextures(myWindow.windowWidth, myWindow.windowHeight);
    buffers.createQuadVAO();

    cam.createCam(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 5.0f, 0.0f, 0.0f);

    mainLoop();
}


void hamoodViewer::mainLoop() {
    while (!glfwWindowShouldClose(myWindow.window)) {

        if (myWindow.reloadModel) {
            model.loadModel(myWindow.windowsFile.lpstrFile);
            buffers.createVertexBuffer(model.vertices);
            buffers.createIndexBuffer(model.indices);
            buffers.createMaterialUBO();
            buffers.createDiffuseTextures(model.diffuseTexturePaths, model.materials);
            myWindow.reloadModel = false;
        }
        if (myWindow.resized) {
            buffers.createFrameBufferTextures(myWindow.windowWidth, myWindow.windowHeight);
            myWindow.resized = false;
        }
        draw();
    }

    glfwTerminate();
}

void hamoodViewer::draw() {
    cam.changeRadius(myWindow.scrollOffset);
    myWindow.scrollOffset = 0.0f;

    glm::mat4 modelTransform(1.0f);
    modelTransform = glm::scale(modelTransform, glm::vec3(2.0f / model.radius));
    modelTransform = glm::translate(modelTransform, -model.centroid);
    cam.rotate_x(myWindow.yaw);
    cam.rotate_y(myWindow.pitch);
    glm::mat4 viewMatrix = cam.get_view_matrix();
    glm::mat4 projMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(myWindow.windowWidth) / static_cast<float>(myWindow.windowHeight), 0.1f, 100.0f);
    //std::cout << glm::to_string(viewMatrix) << std::endl;
    cameraTransformations camMatrixs;
    camMatrixs.model = modelTransform;
    camMatrixs.view = viewMatrix;
    camMatrixs.projections = projMatrix;

    myWindow.yaw = 0.0f;
    myWindow.pitch = 0.0f;

    buffers.updateCameraUBO(camMatrixs);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, buffers.opaqueFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaders.shaderID);

    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        hamoodMaterial& curMaterial = model.materials[mesh.materialIndex];
        buffers.updateMaterialUBO(curMaterial);
        glActiveTexture(GL_TEXTURE0);
        if (curMaterial.diffuseTextureIndex != -1) {
            glBindTexture(GL_TEXTURE_2D, buffers.diffuseTextures[curMaterial.diffuseTextureIndex]);
        }
        // else
            // glBindTexture(GL_TEXTURE_2D, 0);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    glBindFramebuffer(GL_FRAMEBUFFER, buffers.transparentFBO);
    /*GLenum bufs[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1
    };*/

    //glDrawBuffers(2, buffers.transparentDrawBuffers);
    glClearBufferfv(GL_COLOR, 0, &buffers.zeroFillerVec[0]);
    glClearBufferfv(GL_COLOR, 1, &buffers.oneFillerVec[0]);

    glUseProgram(shaders.transparentID);

    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        hamoodMaterial& curMaterial = model.materials[mesh.materialIndex];
        buffers.updateMaterialUBO(curMaterial);
        glActiveTexture(GL_TEXTURE0);
        if (curMaterial.diffuseTextureIndex != -1) {
            glBindTexture(GL_TEXTURE_2D, buffers.diffuseTextures[curMaterial.diffuseTextureIndex]);
        }
        // else
            // glBindTexture(GL_TEXTURE_2D, 0);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }

    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, buffers.opaqueFBO);

    glUseProgram(shaders.compositeID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers.accumTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, buffers.revealTexture);
    glBindVertexArray(buffers.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glUseProgram(shaders.quadID);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindVertexArray(buffers.quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers.opaqueTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glfwSwapBuffers(myWindow.window);
    glfwPollEvents();

}