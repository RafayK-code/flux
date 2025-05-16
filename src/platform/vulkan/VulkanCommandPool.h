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

        VkCommandBuffer AllocateCommandBuffer() const;
        std::vector<VkCommandBuffer> AllocateCommandBuffer(uint32_t count) const;

        void BeginCommandBuffer(VkCommandBuffer commandBuffer) const;
        void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue) const;

        VkCommandPool CommandPool() const { return commandPool_; }

    private:
        VkCommandPool commandPool_;

        // non owning refernece to device
        VkDevice logicalDevice_;
    };
}