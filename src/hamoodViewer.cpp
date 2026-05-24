#include <glad/gl.h>
#include "hamoodViewer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void hamoodViewer::run() {
    myWindow.initGLFW();
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);
    shaders.createShaderProgram();
    model.loadModel(model.defaultModel);
    buffers.createVertexBuffer(model.vertices);
    buffers.createIndexBuffer(model.indices);
    buffers.createCameraUBO();
    buffers.createMaterialUBO();
    buffers.loadRadianceTexture();
    buffers.createDiffuseTextures(model.diffuseTexturePaths, model.materials);
    buffers.createMetallicTexture(model.metallicRoughnessTexturePaths, model.materials);
    buffers.createNormalMapTextures(model.normalMapTexturePaths, model.materials);
    buffers.createAOTextures(model.aoTextuePaths, model.materials);
    buffers.createFrameBuffers();
    buffers.createFrameBufferTextures(myWindow.windowWidth, myWindow.windowHeight);
    buffers.createShadowMap();
    buffers.createQuadVAO();
    buffers.createCubeVAO();
    buffers.loadTextCharacterInfo();
    buffers.createUIVAO(static_cast<float>(myWindow.windowWidth), static_cast<float>(myWindow.windowHeight));
    cam.createCam(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 5.0f, 0.0f, 0.0f);
    buffers.createEnvCubeMap(shaders.cubemapID, shaders.convolutionID, shaders.prefilterID, shaders.brdfShaderID);
    buffers.createMSDFAtlas();
    modelTransform = glm::mat4(1.0f);
    modelTransform = glm::scale(modelTransform, glm::vec3(2.0f / model.radius));
    modelTransform = glm::translate(modelTransform, -model.centroid);
    lightProjection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 25.0f);
    lightView = glm::lookAt(glm::vec3(6.0, 8.0, 6.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraTransformations lightMatrixs;
    lightMatrixs.model = modelTransform;
    lightMatrixs.view = lightView;
    lightMatrixs.projections = lightProjection;
    lightMatrixs.camPos = glm::vec4(1.0f);
    shadowPass(lightMatrixs);
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
            buffers.createMetallicTexture(model.metallicRoughnessTexturePaths, model.materials);
            buffers.createNormalMapTextures(model.normalMapTexturePaths, model.materials);
            buffers.createAOTextures(model.aoTextuePaths, model.materials);
            buffers.createShadowMap();
            modelTransform = glm::mat4(1.0f);
            modelTransform = glm::scale(modelTransform, glm::vec3(2.0f / model.radius));
            modelTransform = glm::translate(modelTransform, -model.centroid);
            lightProjection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 25.0f);
            lightView = glm::lookAt(glm::vec3(6.0, 8.0, 6.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            cameraTransformations lightMatrixs;
            lightMatrixs.model = modelTransform;
            lightMatrixs.view = lightView;
            lightMatrixs.projections = lightProjection;
            lightMatrixs.camPos = glm::vec4(1.0f);
            shadowPass(lightMatrixs);
            myWindow.reloadModel = false;
        }
        if (myWindow.resized) {
            buffers.createFrameBufferTextures(myWindow.windowWidth, myWindow.windowHeight);
            buffers.createUIVAO(static_cast<float>(myWindow.windowWidth), static_cast<float>(myWindow.windowHeight));
            myWindow.resized = false;
        }
        draw();
    }

    glfwTerminate();
}

void hamoodViewer::draw() {

    cam.changeRadius(myWindow.scrollOffset);
    myWindow.scrollOffset = 0.0f;


    cam.rotate_x(myWindow.yaw);
    cam.rotate_y(myWindow.pitch);
    glm::mat4 viewMatrix = cam.get_view_matrix();
    glm::mat4 projMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(myWindow.windowWidth) / static_cast<float>(myWindow.windowHeight), 0.1f, 100.0f);
    //std::cout << glm::to_string(viewMatrix) << std::endl;
    cameraTransformations camMatrixs;
    camMatrixs.model = modelTransform;
    camMatrixs.view = viewMatrix;
    camMatrixs.projections = projMatrix;
    camMatrixs.camPos = glm::vec4(cam.get_eye(), 1.0f);

    myWindow.yaw = 0.0f;
    myWindow.pitch = 0.0f;

    //glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);


    glm::mat4 lightProjView = lightProjection * lightView;

    opaquePass(camMatrixs, lightProjView);

    transparentPass(camMatrixs, lightProjView);

    compositePass();

    //display final result
    glUseProgram(shaders.quadID);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindVertexArray(buffers.quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers.opaqueResolveTexture);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //do ui pass here 
    glm::mat4 uiOrthoProj = glm::ortho(0.0f, static_cast<float>(myWindow.windowWidth), static_cast<float>(myWindow.windowHeight), 0.0f, -1.0f, 1.0f);
    //glm::mat4 uiOrthoProj = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f);
    glUseProgram(shaders.uiID);
    unsigned int transformLoc = glGetUniformLocation(shaders.uiID, "orthProj");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(uiOrthoProj));
    unsigned int clickedLoc = glGetUniformLocation(shaders.uiID, "clicked");
    glUniform1i(clickedLoc, myWindow.clicked);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(buffers.uiVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers.msdfAtlas);
    glDrawElements(GL_TRIANGLES, buffers.indexCount, GL_UNSIGNED_INT, 0);


    glfwSwapBuffers(myWindow.window);
    glfwPollEvents();

}

void hamoodViewer::shadowPass(cameraTransformations& lightMatrix) {
    glViewport(0, 0, buffers.shadowWidth, buffers.shadowHeight);
    // glEnable(GL_CULL_FACE);
     //glCullFace(GL_FRONT);
    buffers.updateCameraUBO(lightMatrix);
    glBindFramebuffer(GL_FRAMEBUFFER, buffers.shadowMapFBO);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    unsigned int transformLoc = glGetUniformLocation(shaders.shadowMapID, "localTransform");

    glUseProgram(shaders.shadowMapID);
    glBindVertexArray(buffers.VAO);

    for (auto& mesh : model.meshes) {
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mesh.localTransform));
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
}

void hamoodViewer::opaquePass(cameraTransformations& camMatrixs, glm::mat4& lightProjView) {
    buffers.updateCameraUBO(camMatrixs);
    glViewport(0, 0, myWindow.windowWidth, myWindow.windowHeight);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, buffers.opaqueFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaders.shaderID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, buffers.irradianceMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, buffers.brdfLUTTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, buffers.prefilterMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, buffers.shadowMap);
    unsigned int transformLoc = glGetUniformLocation(shaders.shaderID, "localTransform");
    unsigned int lightTransformLoc = glGetUniformLocation(shaders.shaderID, "lightSpaceMatrix");
    glUniformMatrix4fv(lightTransformLoc, 1, GL_FALSE, glm::value_ptr(lightProjView));
    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mesh.localTransform));
        hamoodMaterial& curMaterial = model.materials[mesh.materialIndex];
        buffers.updateMaterialUBO(curMaterial);
        if (curMaterial.diffuseTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffers.diffuseTextures[curMaterial.diffuseTextureIndex]);
        }
        if (curMaterial.metallicRoughnessTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, buffers.metallicRoughnessTextures[curMaterial.metallicRoughnessTextureIndex]);
        }

        if (curMaterial.hasNormalMap != -1) {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, buffers.normalMapTextures[curMaterial.hasNormalMap]);
        }

        if (curMaterial.aoTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, buffers.aoMapTextures[curMaterial.aoTextureIndex]);
        }
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }

    //drawing sky box
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glUseProgram(shaders.skyboxID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, buffers.prefilterMap);
    glBindVertexArray(buffers.cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void hamoodViewer::transparentPass(cameraTransformations& camMatrixs, glm::mat4& lightProjView) {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    glBindFramebuffer(GL_FRAMEBUFFER, buffers.transparentFBO);

    glClearBufferfv(GL_COLOR, 0, &buffers.zeroFillerVec[0]);
    glClearBufferfv(GL_COLOR, 1, &buffers.oneFillerVec[0]);

    glUseProgram(shaders.transparentID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, buffers.irradianceMap);
    unsigned int transformLoc = glGetUniformLocation(shaders.transparentID, "localTransform");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, buffers.brdfLUTTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, buffers.prefilterMap);
    unsigned int lightTransformLoc = glGetUniformLocation(shaders.transparentID, "lightSpaceMatrix");
    glUniformMatrix4fv(lightTransformLoc, 1, GL_FALSE, glm::value_ptr(lightProjView));
    glBindVertexArray(buffers.VAO);
    for (auto& mesh : model.meshes) {
        hamoodMaterial& curMaterial = model.materials[mesh.materialIndex];
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mesh.localTransform));
        buffers.updateMaterialUBO(curMaterial);
        if (curMaterial.diffuseTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffers.diffuseTextures[curMaterial.diffuseTextureIndex]);
        }

        if (curMaterial.metallicRoughnessTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, buffers.metallicRoughnessTextures[curMaterial.metallicRoughnessTextureIndex]);
        }

        if (curMaterial.hasNormalMap != -1) {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, buffers.normalMapTextures[curMaterial.hasNormalMap]);
        }
        if (curMaterial.aoTextureIndex != -1) {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, buffers.aoMapTextures[curMaterial.aoTextureIndex]);
        }
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh.indexOffset * sizeof(uint32_t)));
    }
}

void hamoodViewer::compositePass() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, buffers.opaqueFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffers.opaqueResolveFBO);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBlitFramebuffer(
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, buffers.transparentFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffers.transparentResolveFBO);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBlitFramebuffer(
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glBlitFramebuffer(
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        0, 0, myWindow.windowWidth, myWindow.windowHeight,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );

    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, buffers.opaqueResolveFBO);

    glUseProgram(shaders.compositeID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers.accumResolveTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, buffers.revealResolveTexture);
    glBindVertexArray(buffers.quadVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}