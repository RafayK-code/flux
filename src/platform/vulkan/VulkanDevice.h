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

        VkQueue GraphicQueue() const { return graphicsQueue_; }

        VkCommandBuffer CommandBuffer(bool begin);
        void FlushCommandBuffer(VkCommandBuffer commandBuffer);

        const Ref<VulkanPhysicalDevice>& PhysicalDevice() const { return physicalDevice_; }
        VkDevice NativeVulkanDevice() const { return logicalDevice_; }

        void Idle() const;

    private:
        VkDevice logicalDevice_;
        Ref<VulkanPhysicalDevice> physicalDevice_;
        VkPhysicalDeviceFeatures enabledFeatures_;

        VkQueue graphicsQueue_;

        Ref<VulkanCommandPool> commandPool_;
        bool enableDebugMarkers_;
    };
}