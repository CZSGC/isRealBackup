#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "geometry.h"
#include "tool.h"
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "VmaUsage.h"

class IrBuffer
{
  public:
    VkBuffer buffer;
    VmaAllocation all;
    VmaAllocationInfo memHelper;
    VkDeviceSize bufferSize;

    IrBuffer() = default;
    IrBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flages)
    {
        createIrBuffer(size, usage, flages);
    }

    void createIrBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flages)
    {
        bufferSize = size;
        VkBufferCreateInfo bufCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufCreateInfo.size = size;
        bufCreateInfo.usage = usage;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = flages;

        VmaAllocation alloc;
        vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buffer, &all, &memHelper);
    }
};

class IrUniformBuffer : public IrBuffer
{
  public:
    IrUniformBuffer() = default;
    IrUniformBuffer(VkDeviceSize size)
    {
        createIrUniformBuffer(size);
    }
    VkDescriptorBufferInfo descriptorSetBufferInfo;

    void createIrUniformBuffer(VkDeviceSize size)
    {
        createIrBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                           VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                           VMA_ALLOCATION_CREATE_MAPPED_BIT);
        setDescriptorSetBufferInfo();
    }

    void setDescriptorSetBufferInfo()
    {
        descriptorSetBufferInfo.buffer = buffer;
        descriptorSetBufferInfo.offset = 0;
        descriptorSetBufferInfo.range = VK_WHOLE_SIZE;
    }

    template <typename T>

    void copytoUniformBuffer(T ubo)
    {
        createIrUniformBuffer(sizeof(ubo));

        VkMemoryPropertyFlags memPropFlags;
        vmaGetAllocationMemoryProperties(allocator, all, &memPropFlags);

        if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            memcpy(memHelper.pMappedData, &ubo, sizeof(ubo));
            vmaFlushAllocation(allocator, all, 0, VK_WHOLE_SIZE);
            // Check result...

            VkBufferMemoryBarrier bufMemBarrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
            bufMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            bufMemBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
            bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier.buffer = buffer;
            bufMemBarrier.offset = 0;
            bufMemBarrier.size = VK_WHOLE_SIZE;

            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0,
                                 nullptr, 1, &bufMemBarrier, 0, nullptr);
            endSingleTimeCommands(commandBuffer);
        }
        else
        {
            IrBuffer stagingBuffer;
            stagingBuffer.createIrBuffer(sizeof(ubo), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                             VMA_ALLOCATION_CREATE_MAPPED_BIT);

            memcpy(stagingBuffer.memHelper.pMappedData, &ubo, sizeof(ubo));
            vmaFlushAllocation(allocator, stagingBuffer.all, 0, VK_WHOLE_SIZE);

            VkBufferMemoryBarrier bufMemBarrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
            bufMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            bufMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier.buffer = stagingBuffer.buffer;
            bufMemBarrier.offset = 0;
            bufMemBarrier.size = VK_WHOLE_SIZE;
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                 nullptr, 1, &bufMemBarrier, 0, nullptr);

            VkBufferCopy bufCopy = {
                0,           // srcOffset
                0,           // dstOffset,
                sizeof(ubo), // size
            };

            vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, buffer, 1, &bufCopy);

            VkBufferMemoryBarrier bufMemBarrier2 = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
            bufMemBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufMemBarrier2.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT; // We created a uniform buffer
            bufMemBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufMemBarrier2.buffer = buffer;
            bufMemBarrier2.offset = 0;
            bufMemBarrier2.size = VK_WHOLE_SIZE;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0,
                                 0, nullptr, 1, &bufMemBarrier2, 0, nullptr);
            endSingleTimeCommands(commandBuffer);
        }
    }
};

class IrStageBuffer : public IrBuffer
{
  public:
    IrStageBuffer() = default;
    IrStageBuffer(VkDeviceSize size)
    {
        createIrStageBuffer(size);
    }
    void createIrStageBuffer(VkDeviceSize size)
    {
        createIrBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
    }
    template <typename T>

    void loadData(std::vector<T> data)
    {
        memcpy(memHelper.pMappedData, data.data(), data.size() * sizeof(T));
    }

    void tobuffer(IrBuffer &dstbuffer)
    {
        VkCommandBuffer command = beginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(command, buffer, dstbuffer.buffer, 1, &copyRegion);

        endSingleTimeCommands(command);
    }
};
