#pragma once
#include <stdint.h>
#define GLFW_INCLUDE_VULKAN
#include "irbuffer.h"
#include "irdescriptor.h"
#include "irframebuffer.h"
#include "irpipeline.h"
#include "irrenderpass.h"
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

struct UniformOffscreen
{
    alignas(16) glm::mat4 depthMVP;
};

class IrOffscreenResource
{
  public:
    IrOffscreenFrameBuffer frameBuffer;
    IrOffscreenRenderpass renderpass;
    VkSampler depthSampler;
    IrUniformBuffer uniformOffscreen;
    UniformOffscreen uos;
    VkDescriptorImageInfo descriptorImageInfo;
    IrOffscreenPipeline pipeline;
    IrShadowDescriptor shadowDescriptor;

    void createIrOffscreenResource()
    {

        VkSamplerCreateInfo sampler = {};
        sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler.magFilter = VK_FILTER_LINEAR;
        sampler.minFilter = VK_FILTER_LINEAR;
        sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = sampler.addressModeU;
        sampler.addressModeW = sampler.addressModeU;
        sampler.mipLodBias = 0.0f;
        sampler.maxAnisotropy = 1.0f;
        sampler.minLod = 0.0f;
        sampler.maxLod = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        if (vkCreateSampler(device, &sampler, nullptr, &depthSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create offscreen sampler");
        }
        descriptorImageInfo.sampler = depthSampler;
        descriptorImageInfo.imageView = frameBuffer.image.imageView;
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    }

    void createOffscreenUniformBuffer(UniformScreen &ubo)
    {
        // Matrix from light's point of view
        glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 96.0f);
        glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(ubo.lightPos), glm::vec3(0.0f), glm::vec3(0, 1, 0));
        glm::mat4 depthModelMatrix = ubo.model;
        depthProjectionMatrix[1][1] *= -1;

        uos.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
        ubo.depthMVP = uos.depthMVP;
        uniformOffscreen.copytoUniformBuffer(uos);
    }
};