#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include <tiny_obj_loader.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <assimp/Importer.hpp>

#include <assimp/postprocess.h>
#include "model.h"

void hamoodModel::loadModel(const std::string& modelFilePath) {
    std::string modelParentDir = std::filesystem::path(modelFilePath).parent_path().string();
    vertices.clear();
    indices.clear();
    meshes.clear();
    //materials.clear();
    //diffuseTextureNames.clear();
    centroid = glm::vec3(0.0f);
    radius = 0.0f;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    if (scene == nullptr)
        std::cout << "Failed to load model\n";

    glm::mat4 transforms(1.0f);

    processNode(scene->mRootNode, scene, transforms);

    centroid /= vertices.size();

    for (const hamoodMesh& mesh : meshes) {
        for (uint64_t i = mesh.vertexOffset; i < mesh.vertexOffset + mesh.vertexCount; ++i) {
            glm::vec3 vert = vertices[i].pos;
            radius = std::max(radius, glm::length(vert - centroid));
        }
    }
}

void hamoodModel::processNode(aiNode* node, const aiScene* scene, glm::mat4 accumTransforms) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        hamoodMesh tempMesh{};
        tempMesh.indexOffset = indices.size();
        tempMesh.indexCount = 0;
        tempMesh.vertexCount = mesh->mNumVertices;
        tempMesh.vertexOffset = vertices.size();

        for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            aiVector3D vertexCoords = mesh->mVertices[vertexIndex];
            glm::vec2 textureCoords{ 0.5f };
            if (mesh->mTextureCoords[0]) {
                textureCoords.x = mesh->mTextureCoords[0][vertexIndex].x;
                textureCoords.y = mesh->mTextureCoords[0][vertexIndex].y;
            }

            vertices.push_back({ glm::vec3{vertexCoords.x, vertexCoords.y, vertexCoords.z}, textureCoords });
            centroid += vertices.back().pos;
        }

        for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            tempMesh.indexCount += face.mNumIndices;

            for (unsigned int index = 0; index < face.mNumIndices; ++index) {
                indices.push_back(face.mIndices[index] + tempMesh.vertexOffset);
                //indices.push_back(face.mIndices[index]);
            }
        }

        meshes.emplace_back(tempMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, accumTransforms);
    }
}