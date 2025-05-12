#pragma once

#include <platform/vulkan/VulkanPhysicalDevice.h>
#include <platform/vulkan/VulkanCommandPool.h>

namespace flux
{
    class VulkanDevice : public RefCounted
    {
    public:
        VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
        ~VulkanDevice();

        void Destroy();

        VkQueue GraphicQueue() const { return graphicsQueue_; }
#ifdef FLUX_VK_USE_PRESENT_QUEUE
        VkQueue PresentQueue() const { return presentQueue_; }
#endif

        VkCommandBuffer CommandBuffer(bool begin);
        void FlushCommandBuffer(VkCommandBuffer commandBuffer);

        const Ref<VulkanPhysicalDevice>& PhysicalDevice() const { return physicalDevice_; }
        VkDevice NativeVulkanDevice() const { return logicalDevice_; }

    private:
        VkDevice logicalDevice_;
        Ref<VulkanPhysicalDevice> physicalDevice_;
        VkPhysicalDeviceFeatures enabledFeatures_;

        VkQueue graphicsQueue_;
#ifdef FLUX_VK_USE_PRESENT_QUEUE
        VkQueue presentQueue_;
#endif

        Ref<VulkanCommandPool> commandPool_;
        bool enableDebugMarkers_;
    };
}