#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "resourceManager.h"
#include "tool.h"

#include "VmaUsage.h"

class IrImage
{
  public:
    IrImage() = default;
    IrImage(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
            VkImageAspectFlagBits imageAspectFlagBits = VK_IMAGE_ASPECT_COLOR_BIT,
            VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
            uint32_t mipLevels = 1)
    {
        createIrImage(width, height, format, imageAspectFlagBits, usage, numSamples, tiling, mipLevels);
    }
    VkImage image;
    VmaAllocationInfo memHelper;
    VkImageView imageView;
    VmaAllocation all;

    void createIrImage(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                       VkImageAspectFlagBits imageAspectFlagBits = VK_IMAGE_ASPECT_COLOR_BIT,
                       VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                       VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT,
                       VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, uint32_t mipLevels = 1)
    {
        createImage(width, height, format, numSamples, tiling, usage, mipLevels);
        createImageView(format, imageAspectFlagBits);
    }

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits numSamples,
                     VkImageTiling tiling, VkImageUsageFlags usage, uint32_t mipLevels)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.priority = 1.0f;

        vmaCreateImage(allocator, &imageInfo, &allocCreateInfo, &image, &all, &memHelper);
    }

    void createImageView(VkFormat format, VkImageAspectFlagBits imageAspectFlagBits)
    {
        VkImageViewCreateInfo imageViewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        imageViewInfo.image = image;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = format;
        imageViewInfo.subresourceRange.aspectMask = imageAspectFlagBits;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &imageViewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }
    }

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }
    void irDestroyImage()
    {
        vkDestroyImageView(device, imageView, nullptr);
        vmaDestroyImage(allocator, image, all);
    }
};

class IrdepthImage : public IrImage
{
  public:
    IrdepthImage() = default;
    IrdepthImage(uint32_t width, uint32_t height)
    {
        createDepth(width, height);
    }
    void createDepth(uint32_t width, uint32_t height)
    {
        VkFormat format = findDepthFormat();
        createIrImage(width, height, format, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
};

class IrTexture : IrImage
{
  public:
    IrTexture() = default;
    IrTexture(std::vector<uint8_t> &buffer, size_t width, size_t height)
    {
        createTextureByBuffer(buffer, width, height);
    }
    VkDescriptorImageInfo descriptorSetImageInfo;
    VkDescriptorSet descriptorSet;

    void createTextureImage(std::vector<uint8_t> &buffer, size_t width, size_t height)
    {
        createIrImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT,
                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        const VkDeviceSize imageSize = width * height * 4;

        VkBufferCreateInfo stagingBufInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufInfo.size = imageSize;
        stagingBufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo stagingBufAllocCreateInfo = {};
        stagingBufAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingBufAllocCreateInfo.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer stagingBuf = VK_NULL_HANDLE;
        VmaAllocation stagingBufAlloc = VK_NULL_HANDLE;
        VmaAllocationInfo stagingBufAllocInfo = {};
        vmaCreateBuffer(allocator, &stagingBufInfo, &stagingBufAllocCreateInfo, &stagingBuf, &stagingBufAlloc,
                        &stagingBufAllocInfo);

        memcpy(stagingBufAllocInfo.pMappedData, buffer.data(), buffer.size());

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier imgMemBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgMemBarrier.subresourceRange.baseMipLevel = 0;
        imgMemBarrier.subresourceRange.levelCount = 1;
        imgMemBarrier.subresourceRange.baseArrayLayer = 0;
        imgMemBarrier.subresourceRange.layerCount = 1;
        imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgMemBarrier.image = image;
        imgMemBarrier.srcAccessMask = 0;
        imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &imgMemBarrier);

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = width;
        region.imageExtent.height = height;
        region.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(commandBuffer, stagingBuf, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgMemBarrier.image = image;
        imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &imgMemBarrier);

        endSingleTimeCommands(commandBuffer);
    }

    void createDescriptorSetImageInfo()
    {
        descriptorSetImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorSetImageInfo.imageView = imageView;
        descriptorSetImageInfo.sampler = sampler;
    }

    void createDescriptorSet(std::array<VkDescriptorSetLayout,2>& descriptorSetLayout)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout[1];

        if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("create descriptorsets failed");
        }
        VkWriteDescriptorSet writeDescriptorSet{};

        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pImageInfo = &descriptorSetImageInfo;

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
    }

    void createTextureByBuffer(std::vector<uint8_t> &buffer, size_t width, size_t height)
    {
        createTextureImage(buffer, width, height);
        createDescriptorSetImageInfo();
    }
};
