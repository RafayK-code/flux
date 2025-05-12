#pragma once

#include <platform/vulkan/VulkanDevice.h>

#include <vulkan/vulkan.h>

namespace flux
{
    struct VulkanSwapChainCreateProps
    {
        uint32_t width;
        uint32_t height;
        bool vSync;
    };

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapChainCreateProps& props);
        ~VulkanSwapChain();

        void BeginFrame();
        void Present();

    private:
        VkInstance instance_;
        Ref<VulkanDevice> device_;

        VkSurfaceKHR surface_;
    };
}