#pragma once

#include <platform/vulkan/VulkanPhysicalDevice.h>
#include <platform/vulkan/VulkanCommandPool.h>
#include <platform/vulkan/VulkanDescriptorPool.h>

namespace flux
{
    class VulkanDevice : public RefCounted
    {
    public:
        VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
        ~VulkanDevice();

        VkQueue GraphicQueue() const { return graphicsQueue_; }

        VkCommandBuffer AllocateCommandBuffer() const;
        void FlushCommandBuffer(VkCommandBuffer commandBuffer) const;

        const Ref<VulkanPhysicalDevice>& PhysicalDevice() const { return physicalDevice_; }
        VkDevice NativeVulkanDevice() const { return logicalDevice_; }

        const Ref<VulkanCommandPool> CommandPool() const { return commandPool_; }
        const Ref<VulkanDescriptorPool> DescriptorPool() const { return descriptorPool_; }

        void Idle() const;

    private:
        VkDevice logicalDevice_;
        Ref<VulkanPhysicalDevice> physicalDevice_;
        VkPhysicalDeviceFeatures enabledFeatures_;

        VkQueue graphicsQueue_;

        Ref<VulkanCommandPool> commandPool_;
        Ref<VulkanDescriptorPool> descriptorPool_;
        bool enableDebugMarkers_;
    };
}