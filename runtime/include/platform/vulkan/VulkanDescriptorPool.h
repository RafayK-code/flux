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
        std::vector<VkDescriptorSet> AllocateDescriptorSet(const std::vector<VkDescriptorSetLayout>& layouts) const;

        void FreeDescriptorSet(VkDescriptorSet descriptorSet) const;
        void FreeDescriptorSet(const std::vector<VkDescriptorSet>& descriptorSet) const;

        VkDescriptorPool NativeDescriptorPool() const { return descriptorPool_; }

    private:
        VkDescriptorPool descriptorPool_;

        // non owning refernece to device
        VkDevice logicalDevice_;
    };
}