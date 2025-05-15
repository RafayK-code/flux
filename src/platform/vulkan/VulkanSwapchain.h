#pragma once

#include <platform/vulkan/VulkanDevice.h>
#include <platform/vulkan/VulkanAllocator.h>

#include <vulkan/vulkan.h>

namespace flux
{
    struct VulkanSwapchainCreateProps
    {
        uint32_t width;
        uint32_t height;
        bool vSync;
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapchainCreateProps& props);
        ~VulkanSwapchain();

        void BeginFrame();
        void Present();

    private:
        void FindImageFormatAndColorSpace();

    private:
        VkInstance instance_;
        Ref<VulkanDevice> device_;

        uint32_t queueFamilyIndex_;

        VkFormat colorFormat_;
        VkColorSpaceKHR colorSpace_;

        VkSwapchainKHR swapchain_;
        uint32_t imageCount_;
        std::vector<VkImage> vulkanImages_;

        struct SwapchainImage
        {
            VkImage image = nullptr;
            VkImageView imageView = nullptr;
        };

        std::vector<SwapchainImage> images_;

        struct DepthStencilAttachment
        {
            VkImage image = nullptr;
            VmaAllocation memoryAlloc = nullptr;
            VkImageView imageView = nullptr;
        };

        DepthStencilAttachment depthStencil_;

        std::vector<VkFramebuffer> framebuffers_;

        struct SwapchainCommandBuffer
        {
            VkCommandPool commandPool = nullptr;
            VkCommandBuffer commandBuffer = nullptr;
        };

        std::vector<SwapchainCommandBuffer> commandBuffers_;

        std::vector<VkSemaphore> imageAvailableSemaphores_;
        std::vector<VkSemaphore> renderFinishedSemaphores_;

        std::vector<VkFence> waitFences_;

        VkRenderPass renderPass_;
        uint32_t currentFrameIndex_;
        uint32_t currentImageIndex_;

        uint32_t width_;
        uint32_t height_;

        VkSurfaceKHR surface_;
    };
}