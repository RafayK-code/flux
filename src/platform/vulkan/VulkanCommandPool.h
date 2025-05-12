#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanCommandPool : public RefCounted
    {
    public:
        VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex);
        ~VulkanCommandPool();

        VkCommandBuffer AllocateCommandBuffer(bool begin) const;
        void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue) const;

        VkCommandPool CommandPool() const { return commandPool_; }

    private:
        VkCommandPool commandPool_;

        // non owning refernece to device
        VkDevice logicalDevice_;
    };
}