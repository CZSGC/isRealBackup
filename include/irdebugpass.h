#pragma once
#include <stdint.h>
#define GLFW_INCLUDE_VULKAN
#include "irbuffer.h"
#include "iroffscreen.h"
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "irdescriptor.h"
#include "irpipeline.h"
#include <glm/glm.hpp>

class IrDebugPass
{
  public:
    IrDebugPipeline pipeline;
    IrDebugDescriptor debugDescriptor;
};