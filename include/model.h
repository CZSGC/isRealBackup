#pragma once
#include "geometry.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VmaUsage.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>

#include <gl/GL.h>

#include "tglfUsage.h"

#define DEFAULT_FENCE_TIMEOUT 60000000000ull

#include "irImage.h"
#include "resourceManager.h"
#include "tool.h"
#include <unordered_map>



inline void indexBufferInsert32(std::vector<uint32_t>& indices,size_t vertexStart, const uint8_t* indexData, size_t indexCount) {

    const uint32_t* buf = reinterpret_cast<const uint32_t*>(indexData);
    for (size_t index = 0; index < indexCount; index++) {
        indices.push_back(static_cast<uint32_t>(buf[index] + vertexStart));
    }

}

inline void indexBufferInsert16(std::vector<uint32_t>& indices, size_t vertexStart, const uint8_t* indexData, size_t indexCount) {
    const uint16_t* buf = reinterpret_cast<const uint16_t*>(indexData);
    for (size_t index = 0; index < indexCount; index++) {
        indices.push_back(static_cast<uint32_t>(buf[index] + vertexStart));
    }
}

inline void indexBufferInsert8(std::vector<uint32_t>& indices, size_t vertexStart, const uint8_t* indexData, size_t indexCount) {
    const uint8_t* buf = reinterpret_cast<const uint8_t*>(indexData);
    for (size_t index = 0; index < indexCount; index++) {
        indices.push_back(static_cast<uint32_t>(buf[index] + vertexStart));
    }
}


inline glm::mat4 getMatrix(tinygltf::Node &node)
{
    glm::mat4 matrix(1.0f);

    if (node.translation.size() == 3)
    {
        matrix = glm::translate(matrix, glm::vec3(glm::make_vec3(node.translation.data())));
    }
    if (node.rotation.size() == 4)
    {
        glm::quat q = glm::make_quat(node.rotation.data());
        matrix *= glm::mat4(q);
    }
    if (node.scale.size() == 3)
    {
        matrix = glm::scale(matrix, glm::vec3(glm::make_vec3(node.scale.data())));
    }
    if (node.matrix.size() == 16)
    {
        matrix = glm::make_mat4x4(node.matrix.data());
    };
    return matrix;
}

inline void loadMesh(tinygltf::Mesh &mesh, std::vector<Vertex> &vertexs, std::vector<uint32_t> &indices, int &firstIndex, std::unordered_map<int, int> &firstIndexs, tinygltf::Node& node)
{
    for (size_t k = 0; k < mesh.primitives.size(); k++)
    {

        tinygltf::Primitive primitive = mesh.primitives[k];

        firstIndexs[primitive.indices] = firstIndex;

        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];
        tinygltf::Buffer indexBuffer = model.buffers[indexBufferView.buffer];

        const uint8_t *indexData = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
        size_t indexCount = indexAccessor.count;
        size_t indexLength = indexBufferView.byteLength;
        firstIndex += indexCount;
        size_t vertexStart = static_cast<size_t>(vertexs.size());


        switch (indexAccessor.componentType)
        {
        case 5120: // BYTE
            indexBufferInsert8(indices, vertexStart, indexData, indexCount);
            break;
        case 5121: // UNSIGNED_BYTE
            indexBufferInsert8(indices, vertexStart, indexData, indexCount);
            break;
        case 5122: // SHORT
            indexBufferInsert16(indices, vertexStart, indexData, indexCount);
            break;
        case 5123: // UNSIGNED_SHORT
            indexBufferInsert16(indices, vertexStart, indexData, indexCount);
            break;
        case 5125: // UNSIGNED_INT
            indexBufferInsert32(indices, vertexStart, indexData, indexCount);

            break;
        case 5126: // FLOAT
            indexBufferInsert32(indices, vertexStart, indexData, indexCount);

            break;
        default:
            throw std::runtime_error("Unknown glTF component type.");
        }

        //indices.insert(indices.end(), indexData, indexData + indexLength);

        const float *positionBuffer = nullptr;
        const float *normalsBuffer = nullptr;
        const float *texCoordsBuffer = nullptr;

        size_t vertexCount = 0;

        // Get buffer data for vertex positions
        if (primitive.attributes.find("POSITION") != primitive.attributes.end())
        {
            const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
            positionBuffer = reinterpret_cast<const float *>(
                &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            vertexCount = accessor.count;
        }
        // Get buffer data for vertex normals
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
        {
            const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
            const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
            normalsBuffer = reinterpret_cast<const float *>(
                &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        }
        // Get buffer data for vertex texture coordinates
        // glTF supports multiple sets, we only load the first one
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
        {
            const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
            const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
            texCoordsBuffer = reinterpret_cast<const float *>(
                &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        }

        for (size_t v = 0; v < vertexCount; v++)
        {
            Vertex vert{};
            vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);


            vert.pos = getMatrix(node) * glm::vec4(vert.pos, 1.0f);


            if (vert.pos.x < xl) {
                xl = vert.pos.x;
            }
            else if (vert.pos.x > xr) {
                xr = vert.pos.x;
            }

            if (vert.pos.y < yb) {
                yb = vert.pos.y;
            }
            else if (vert.pos.y > yt) {
                yt = vert.pos.y;
            }

            if (vert.pos.z < zb) {
                zb = vert.pos.z;
            }
            else if (vert.pos.z > zf) {
                zf = vert.pos.z;
            }
            vert.flags = 0;

            if (primitive.material == -1) {
                vert.flags |= 1;
            }


            vert.normal =
                glm::normalize(getMatrix(node) * glm::vec4(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)),1.0f));
            vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
            vert.color = glm::vec3(1.0f);
            vertexs.push_back(vert);
        }
    }
}

inline void loadNode(tinygltf::Node &node, std::vector<Vertex> &vertexs, std::vector<uint32_t> &indices, int &firstIndex, std::unordered_map<int, int> &firstIndexs)
{
    if (node.mesh != -1) {
            tinygltf::Mesh mesh = model.meshes[node.mesh];
            loadMesh(mesh, vertexs, indices, firstIndex, firstIndexs,node);
    }


    if (node.children.size() > 0)
    {
        for (size_t i = 0; i < node.children.size(); i++)
        {

            loadNode(model.nodes[node.children[i]], vertexs, indices, firstIndex, firstIndexs);
        }
    }
}

inline void loadModel(const std::string filename, std::vector<Vertex> &vertexs, std::vector<uint32_t> &indices, int &firstIndex, std::unordered_map<int, int> &firstIndexs)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    //bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];

    for (size_t i = 0; i < scene.nodes.size(); i++)
    {
        tinygltf::Node node = model.nodes[scene.nodes[i]];
        loadNode(node, vertexs, indices, firstIndex, firstIndexs);
    }
}

inline void loadImages(std::vector<IrTexture> &Textures)
{
    for (auto glTFImage : model.images)
    {
        IrTexture texture;
        std::vector<uint8_t> imagedata;
        if (glTFImage.component == 3)
        {
            imagedata.resize(glTFImage.width * glTFImage.height * 4);
            for (size_t j = 0; j < glTFImage.width * glTFImage.height; j++)
            {
                imagedata.insert(imagedata.end(), &(glTFImage.image[3 * j]), &(glTFImage.image[3 * j + 3]));
                imagedata.push_back(static_cast<uint8_t>(0xFF));
            }
        }
        else
        {
            imagedata.insert(imagedata.end(), &(glTFImage.image[0]),
                             &(glTFImage.image[0]) + glTFImage.width * glTFImage.height * 4);
        }
        texture.createTextureByBuffer(imagedata, glTFImage.width, glTFImage.height);
        Textures.push_back(texture);
    }
}

inline void loadTexture(std::vector<uint32_t> textures)
{
    textures.resize(model.textures.size());
    for (size_t i = 0; i < model.textures.size(); i++)
    {
        textures[i] = model.textures[i].source;
    }
}

// void loadMaterials(tinygltf::Model& model,std::vector<Material> materials){
//     materials.resize(model.materials.size());
//     for (size_t i = 0; i < model.materials.size(); i++) {
//         // We only read the most basic properties required for our sample
//         tinygltf::Material glTFMaterial = model.materials[i];
//         // Get the base color factor
//         if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
//             materials[i].baseColorFactor =
//             glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
//         }
//         // Get base color texture index
//         if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
//             materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
//         }
//     }
// }

class ModelLoader
{
};