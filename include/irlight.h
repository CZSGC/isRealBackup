#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

class irLight
{
  public:
    glm::vec3 pos;
    glm::vec3 center;
    glm::vec3 up;
};