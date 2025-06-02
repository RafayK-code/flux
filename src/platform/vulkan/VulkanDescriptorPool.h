#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanDescriptorPool : public RefCounted
    {
    public:
        VulkanDescriptorPool(VkDevice device);
        ~VulkanDescriptorPool();

        VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout) const;
        void FreeDescriptorSet(VkDescriptorSet descriptorSet) const;

        VkDescriptorPool NativeDescriptorPool() const { return descriptorPool_; }

    private:
        VkDescriptorPool descriptorPool_;

        // non owning refernece to device
        VkDevice logicalDevice_;
    };
}