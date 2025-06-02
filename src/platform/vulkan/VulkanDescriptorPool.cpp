#include <pch.h>

#include <platform/vulkan/VulkanDescriptorPool.h>

namespace flux
{
    VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device)
        : logicalDevice_(device)
    {
        std::array<VkDescriptorPoolSize, 3> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = 1000;

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = 1000;

        poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[2].descriptorCount = 1000;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000; // Max number of descriptor sets that can be allocated from this pool
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Optional, allows freeing sets individually

        VkResult result = vkCreateDescriptorPool(logicalDevice_, &poolInfo, nullptr, &descriptorPool_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool");
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
    }

    VkDescriptorSet VulkanDescriptorPool::AllocateDescriptorSet(VkDescriptorSetLayout layout) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool_;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet descriptorSet;
        VkResult result = vkAllocateDescriptorSets(logicalDevice_, &allocInfo, &descriptorSet);

        DBG_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set");
        return descriptorSet;
    }

    void VulkanDescriptorPool::FreeDescriptorSet(VkDescriptorSet descriptorSet) const
    {
        vkFreeDescriptorSets(logicalDevice_, descriptorPool_, 1, &descriptorSet);
    }
}