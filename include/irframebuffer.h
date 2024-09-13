#pragma once
#define GLFW_INCLUDE_VULKAN
#include "irImage.h"
#include "irrenderpass.h"
#include "irswapchain.h"
#include "tool.h"
#include <GLFW/glfw3.h>

class IrFrameBuffer
{
  public:
    std::vector<VkFramebuffer> swapChainFramebuffers;
    void createFramebuffers(IrSwapChain &swapChain, IrRenderpass &renderPass)
    {
        swapChainFramebuffers.resize(swapChain.swapChainImages.size());

        for (size_t i = 0; i < swapChain.swapChainImages.size(); i++)
        {
            std::array<VkImageView, 2> attachments = {swapChain.swapChainImageViews[i], swapChain.depthImage.imageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChain.swapChainExtent.width;
            framebufferInfo.height = swapChain.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
};

class IrOffscreenFrameBuffer
{
  public:
    IrImage image;
    VkFramebuffer framebuffer;
    void createFrameBuffer(uint32_t shadowMapize, IrOffscreenRenderpass & offscreenRenderPass)
    {
        image.createIrImage(shadowMapize, shadowMapize, VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT,
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);



        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = offscreenRenderPass.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &image.imageView;
        framebufferInfo.width = shadowMapize;
        framebufferInfo.height = shadowMapize;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
};