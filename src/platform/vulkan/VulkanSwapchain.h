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
        bool vsync;
    };

    static inline constexpr uint32_t VulkanFramesInFlight() { return 3; }

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapchainCreateProps& props);
        ~VulkanSwapchain();

        inline uint32_t ImageCount() const { return imageCount_; }

        inline uint32_t Width() const { return width_; }
        inline uint32_t Height() const { return height_; }

        inline VkRenderPass RenderPass() const { return renderPass_; }

        inline VkFramebuffer GetFramebuffer(uint32_t index) { return framebuffers_[index]; }
        inline VkFramebuffer CurrentFramebuffer() { return framebuffers_[currentImageIndex_]; }

        uint32_t AcquireNextImage();
        void Present(const VkCommandBuffer* commandBuffers);

        uint32_t CurrentImageIndex() const { return currentImageIndex_; }

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

        struct DepthImage
        {
            VkImage image = nullptr;
            VmaAllocation memoryAlloc = nullptr;
            VkImageView imageView = nullptr;
        };

        std::vector<SwapchainImage> images_;
        std::vector<DepthImage> depthImages_;

        std::vector<VkFramebuffer> framebuffers_;

        std::vector<VkSemaphore> imageAvailableSemaphores_;
        std::vector<VkSemaphore> renderFinishedSemaphores_;

        std::vector<VkFence> imagesInFlight_;
        std::vector<VkFence> inFlightFences_;

        VkRenderPass renderPass_;
        uint32_t currentFrameIndex_;
        uint32_t currentImageIndex_;

        uint32_t width_;
        uint32_t height_;
        bool vsync_;

        VkSurfaceKHR surface_;

        friend class VulkanContext;
    };
}