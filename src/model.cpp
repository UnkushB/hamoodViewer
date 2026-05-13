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
    materials.clear();
    diffuseTexturePaths.clear();
    centroid = glm::vec3(0.0f);
    radius = 0.0f;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    if (scene == nullptr)
        std::cout << "Failed to load model\n";

    glm::mat4 transforms(1.0f);

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial& mat = *scene->mMaterials[i];

        hamoodMaterial tempMat{};

        if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString texturePath;
            mat.GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            std::string fullTexturePath = modelParentDir + "/" + texturePath.C_Str();
            //diffuseTextures.emplace(fullTexturePath, -1);
            diffuseTexturePaths[fullTexturePath].push_back(i);
        }

        aiColor3D diffuseColor;
        if (mat.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != AI_SUCCESS) {
            diffuseColor = aiColor3D(0.5, 0.2, 0.8);
        }
        tempMat.diffuse = glm::pow(glm::vec3{ diffuseColor.r, diffuseColor.g, diffuseColor.b }, glm::vec3(2.2f));


        float opacity;
        if (mat.Get(AI_MATKEY_OPACITY, opacity) != AI_SUCCESS)
            opacity = 1.0f;
        tempMat.opacity = opacity;

        materials.emplace_back(tempMat);
    }

    processNode(scene->mRootNode, scene, transforms);

    centroid /= vertices.size();

    for (const hamoodMesh& mesh : meshes) {
        for (uint64_t i = mesh.vertexOffset; i < mesh.vertexOffset + mesh.vertexCount; ++i) {
            glm::vec3 vert = vertices[i].pos;
            glm::vec4 pos = mesh.localTransform * glm::vec4(vert, 1.0f);
            radius = std::max(radius, glm::length(glm::vec3(pos) - centroid));
        }
    }
}

void hamoodModel::processNode(aiNode* node, const aiScene* scene, glm::mat4 accumTransforms) {
    aiMatrix4x4& tempTransform = node->mTransformation;

    glm::mat4 transform{
  { tempTransform.a1, tempTransform.b1, tempTransform.c1, tempTransform.d1 },
  { tempTransform.a2, tempTransform.b2, tempTransform.c2, tempTransform.d2 },
  { tempTransform.a3, tempTransform.b3, tempTransform.c3, tempTransform.d3 },
  { tempTransform.a4, tempTransform.b4, tempTransform.c4, tempTransform.d4 }
    };

    accumTransforms = accumTransforms * transform;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        hamoodMesh tempMesh{};
        tempMesh.indexOffset = indices.size();
        tempMesh.indexCount = 0;
        tempMesh.vertexCount = mesh->mNumVertices;
        tempMesh.vertexOffset = vertices.size();
        tempMesh.materialIndex = mesh->mMaterialIndex;
        tempMesh.localTransform = accumTransforms;

        for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            aiVector3D vertexCoords = mesh->mVertices[vertexIndex];
            glm::vec2 textureCoords{ 0.5f };
            if (mesh->mTextureCoords[0]) {
                textureCoords.x = mesh->mTextureCoords[0][vertexIndex].x;
                textureCoords.y = mesh->mTextureCoords[0][vertexIndex].y;
            }

            vertices.push_back({ glm::vec3{vertexCoords.x, vertexCoords.y, vertexCoords.z}, textureCoords });
            //vertices.push_back({ glm::vec3(accumTransforms * glm::vec4{vertexCoords.x, vertexCoords.y, vertexCoords.z, 1.0f}), textureCoords });
            glm::vec4 pos = accumTransforms * glm::vec4{ vertexCoords.x, vertexCoords.y, vertexCoords.z, 1.0f };
            //centroid += vertices.back().pos;
            centroid += glm::vec3(pos);
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