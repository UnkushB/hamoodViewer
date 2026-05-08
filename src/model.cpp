#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "model.h"

void hamoodModel::loadModel(const std::string& modelFilePath) {
    vertices.clear();
    indices.clear();
    meshes.clear();
    materials.clear();
    diffuseTextureNames.clear();
    centroid = glm::vec3{ 0.0f };
    radius = 0.0f;

    std::string modelParentDir = std::filesystem::path(modelFilePath).parent_path().string();
    tinyobj::ObjReaderConfig reader_config;

    std::cout << "model parent dir: " << modelParentDir << std::endl;

    centroid = glm::vec3{ 0.0f };
    radius = 0.0f;

    reader_config.mtl_search_path = modelParentDir; // Path to material files
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(modelFilePath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& tinyMaterials = reader.GetMaterials();

    std::unordered_map<int, std::vector<uint32_t>> perMaterialIndices;
    uint32_t index = 0;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            int materialID = -1;
            if (!shapes[s].mesh.material_ids.empty())
                materialID = shapes[s].mesh.material_ids[f];
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                tinyobj::real_t tx = 0.0f;
                tinyobj::real_t ty = 0.0f;
                if (idx.texcoord_index >= 0) {
                    tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }


                vertices.push_back({ glm::vec3{vx, vy, vz}, glm::vec2{tx, ty} });
                centroid += vertices.back().pos;
                perMaterialIndices[materialID].push_back(index);
                ++index;
            }
            index_offset += fv;
        }

    }

    for (size_t m = 0; m < tinyMaterials.size(); ++m) {
        material mat{};
        auto& diffuse = tinyMaterials[m].diffuse;
        //auto& opacity = tinyMaterials[m].dissolve;
        mat.diffuse = glm::vec4{ diffuse[0], diffuse[1], diffuse[2], -1 };
        std::string fullTexturePath = modelParentDir + '\\' + tinyMaterials[m].diffuse_texname;
        diffuseTextureNames[fullTexturePath].push_back(m);
        materials.emplace_back(mat);
    }

    material defaultMat{};
    defaultMat.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, -1);
    materials.emplace_back(defaultMat);

    //std::cout << "materials size: " << materials.size() << std::endl;

    centroid /= vertices.size();

    for (auto& x : vertices) {
        glm::vec3 vert = x.pos;
        radius = std::max(radius, glm::length(vert - centroid));
    }

    for (auto& x : perMaterialIndices) {
        mesh mesh{};
        mesh.indexOffset = uint32_t(indices.size());
        mesh.indexCount = uint32_t(x.second.size());
        mesh.materialIndex = x.first;
        //std::cout << "mesh index: " << x.first << std::endl;
        indices.insert(indices.end(), x.second.begin(), x.second.end());

        meshes.emplace_back(mesh);
    }

    std::cout << meshes.size() << " : " << vertices.size() << " : " << indices.size() << std::endl;
}