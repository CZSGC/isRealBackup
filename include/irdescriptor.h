#pragma once

#include "irImage.h"
#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
class IrShadowRenderDescriptor
{
  public:
    std::array<VkDescriptorSetLayout, 2> shadowRenderDescriptorSetLayout;
    VkDescriptorSet shadowRenderDescriptorSet;

    void createShadowRenderDescriptorSetLayouts()
    {
        VkDescriptorSetLayoutBinding uniformLayoutBinding{};
        uniformLayoutBinding.binding = 0;
        uniformLayoutBinding.descriptorCount = 1;
        uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformLayoutBinding.pImmutableSamplers = nullptr;
        uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding shadowSamplerLayoutBinding{};
        shadowSamplerLayoutBinding.binding = 1;
        shadowSamplerLayoutBinding.descriptorCount = 1;
        shadowSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowSamplerLayoutBinding.pImmutableSamplers = nullptr;
        shadowSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> Bindings = {uniformLayoutBinding, shadowSamplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo createLayoutInfo{};

        createLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createLayoutInfo.bindingCount = Bindings.size();
        createLayoutInfo.pBindings = Bindings.data();

        VkDescriptorSetLayoutCreateInfo imageCreateLayoutInfo{};

        imageCreateLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        imageCreateLayoutInfo.bindingCount = 1;
        imageCreateLayoutInfo.pBindings = &samplerLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &createLayoutInfo, nullptr, &shadowRenderDescriptorSetLayout[0]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        if (vkCreateDescriptorSetLayout(device, &imageCreateLayoutInfo, nullptr, &shadowRenderDescriptorSetLayout[1]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    void createShadowRenderDescriptorSet(VkDescriptorBufferInfo &uniformBufferInfo,
                                         VkDescriptorImageInfo &shadowImageInfo)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &shadowRenderDescriptorSetLayout[0];

        if (vkAllocateDescriptorSets(device, &allocInfo, &shadowRenderDescriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("create descriptorsets failed");
        }

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        VkWriteDescriptorSet uniformWriteDescriptorSet{};
        uniformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformWriteDescriptorSet.dstSet = shadowRenderDescriptorSet;
        uniformWriteDescriptorSet.dstBinding = 0;
        uniformWriteDescriptorSet.dstArrayElement = 0;
        uniformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformWriteDescriptorSet.descriptorCount = 1;
        uniformWriteDescriptorSet.pBufferInfo = &uniformBufferInfo;

        writeDescriptorSets.push_back(uniformWriteDescriptorSet);

        VkWriteDescriptorSet shadowSamplerWriteDescriptorSet{};
        shadowSamplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        shadowSamplerWriteDescriptorSet.dstSet = shadowRenderDescriptorSet;
        shadowSamplerWriteDescriptorSet.dstBinding = 1;
        shadowSamplerWriteDescriptorSet.dstArrayElement = 0;
        shadowSamplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowSamplerWriteDescriptorSet.descriptorCount = 1;
        shadowSamplerWriteDescriptorSet.pImageInfo = &shadowImageInfo;

        writeDescriptorSets.push_back(shadowSamplerWriteDescriptorSet);

        vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }
};

class IrShadowDescriptor
{
  public:
    VkDescriptorSetLayout shadowDescriptorSetLayout;
    VkDescriptorSet shadowDescriptorSet;

    void createShadowDescriptorSetLayouts()
    {
        VkDescriptorSetLayoutBinding uniformLayoutBinding{};
        uniformLayoutBinding.binding = 0;
        uniformLayoutBinding.descriptorCount = 1;
        uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformLayoutBinding.pImmutableSamplers = nullptr;
        uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        std::array<VkDescriptorSetLayoutBinding, 1> Bindings = {uniformLayoutBinding};

        VkDescriptorSetLayoutCreateInfo createLayoutInfo{};

        createLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createLayoutInfo.bindingCount = Bindings.size();
        createLayoutInfo.pBindings = Bindings.data();

        if (vkCreateDescriptorSetLayout(device, &createLayoutInfo, nullptr, &shadowDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    void createShadowDescriptorSet(VkDescriptorBufferInfo &uniformBufferInfo)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &shadowDescriptorSetLayout;

        if (vkAllocateDescriptorSets(device, &allocInfo, &shadowDescriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("create descriptorsets failed");
        }

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        VkWriteDescriptorSet uniformWriteDescriptorSet{};
        uniformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformWriteDescriptorSet.dstSet = shadowDescriptorSet;
        uniformWriteDescriptorSet.dstBinding = 0;
        uniformWriteDescriptorSet.dstArrayElement = 0;
        uniformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformWriteDescriptorSet.descriptorCount = 1;
        uniformWriteDescriptorSet.pBufferInfo = &uniformBufferInfo;

        writeDescriptorSets.push_back(uniformWriteDescriptorSet);

        vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }
};

class IrDebugDescriptor
{
  public:
    VkDescriptorSetLayout debugDescriptorSetLayout;
    VkDescriptorSet debugDescriptorSet;

    void createDebugDescriptorSetLayouts()
    {
        VkDescriptorSetLayoutBinding uniformLayoutBinding{};
        uniformLayoutBinding.binding = 0;
        uniformLayoutBinding.descriptorCount = 1;
        uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformLayoutBinding.pImmutableSamplers = nullptr;
        uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding shadowSamplerLayoutBinding{};
        shadowSamplerLayoutBinding.binding = 1;
        shadowSamplerLayoutBinding.descriptorCount = 1;
        shadowSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowSamplerLayoutBinding.pImmutableSamplers = nullptr;
        shadowSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> Bindings = {uniformLayoutBinding, shadowSamplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo createLayoutInfo{};

        createLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createLayoutInfo.bindingCount = Bindings.size();
        createLayoutInfo.pBindings = Bindings.data();

        if (vkCreateDescriptorSetLayout(device, &createLayoutInfo, nullptr, &debugDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createDebugDescriptorSet(VkDescriptorBufferInfo &uniformBufferInfo, VkDescriptorImageInfo &shadowImageInfo)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &debugDescriptorSetLayout;

        if (vkAllocateDescriptorSets(device, &allocInfo, &debugDescriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("create descriptorsets failed");
        }

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        VkWriteDescriptorSet uniformWriteDescriptorSet{};
        uniformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformWriteDescriptorSet.dstSet = debugDescriptorSet;
        uniformWriteDescriptorSet.dstBinding = 0;
        uniformWriteDescriptorSet.dstArrayElement = 0;
        uniformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformWriteDescriptorSet.descriptorCount = 1;
        uniformWriteDescriptorSet.pBufferInfo = &uniformBufferInfo;

        writeDescriptorSets.push_back(uniformWriteDescriptorSet);

        VkWriteDescriptorSet shadowSamplerWriteDescriptorSet{};
        shadowSamplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        shadowSamplerWriteDescriptorSet.dstSet = debugDescriptorSet;
        shadowSamplerWriteDescriptorSet.dstBinding = 1;
        shadowSamplerWriteDescriptorSet.dstArrayElement = 0;
        shadowSamplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowSamplerWriteDescriptorSet.descriptorCount = 1;
        shadowSamplerWriteDescriptorSet.pImageInfo = &shadowImageInfo;

        writeDescriptorSets.push_back(shadowSamplerWriteDescriptorSet);

        vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }
};
