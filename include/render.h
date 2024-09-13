#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "tglfUsage.h"

#include "irbuffer.h"
#include "irdescriptor.h"
#include "irframebuffer.h"
#include "irpipeline.h"
#include "irrenderpass.h"
#include "irswapchain.h"

#include "model.h"

#include "VmaUsage.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "geometry.h"
#include "irdebugpass.h"
#include "iroffscreen.h"
#include "tool.h"

#include <unordered_map>

class Render
{
  public:
    void run();
    Render() = default;
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);
    void initVulkan();
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void mainLoop();
    void cleanup();
    void createInstance();
    void createUniformBuffer();
    void drawNode(tinygltf::Node node, VkPipelineLayout pipelineLayout);
    void draw(VkPipelineLayout pipelineLayout);
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    void drawFrame();
    void createSyncObjects();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createIndexBuffer();
    void createVertexBuffer();
    VkSampleCountFlagBits getMaxUsableSampleCount();
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    bool hasStencilComponent(VkFormat format);
    void createSurface();
    void cpyBuffer();
    void createCommandBuffer();
    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void createDescriptorSet();
    void createRenderPass();
    void createFrameBuffer();
    void createPipeLine();
    void createOffscreenResource();
    void createDescriptorSetLayout();
    

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    IrDebugPass debugpass;
    IrOffscreenResource offscreen;

    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    IrBuffer indexBuffer;
    IrBuffer vertexBuffer;
    IrFrameBuffer frameBuffer;

    IrSwapChain swapchain;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    IrRenderpass renderpass;

    IrPipeline pipeline;
    IrShadowRenderPipeline shadowRenderPipeline;

    IrShadowRenderDescriptor shadowRenderDescriptor;

    IrUniformBuffer uniformBuffer;
    UniformScreen ubo;

    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    std::vector<IrTexture> irTextures;

    int firstIndex = 0;
    std::unordered_map<int, int> firstIndexs;

    bool framebufferResized = false;
};