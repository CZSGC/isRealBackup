#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <stdint.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
    uint32_t flags;

    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && uv == other.uv;
    }
};

namespace std
{
template <> struct hash<Vertex>
{
    size_t operator()(Vertex const &vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.uv) << 1) ^ ((hash<glm::vec3>()(vertex.color) >> 1) >> 1);
    }
};
} // namespace std
