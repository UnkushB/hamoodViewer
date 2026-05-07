#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "model.h"

void hamoodModel::loadModel(std::string&& modelFilePath) {
    std::string modelParentDir = std::filesystem::path(defaultModel).parent_path().string();
    tinyobj::ObjReaderConfig reader_config;

    centroid = glm::vec3{ 0.0f };
    radius = 0.0f;

    reader_config.mtl_search_path = modelParentDir; // Path to material files
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(defaultModel, reader_config)) {
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

                vertices.push_back({ glm::vec3{vx, vy, vz} });
                centroid += vertices.back().pos;
                perMaterialIndices[materialID].push_back(index);
                ++index;
            }
            index_offset += fv;
        }

    }

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

        indices.insert(indices.end(), x.second.begin(), x.second.end());

        meshes.emplace_back(mesh);
    }

    std::cout << meshes.size() << " : " << vertices.size() << " : " << indices.size() << std::endl;
}