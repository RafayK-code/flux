#include <fxpch.h>

#include <platform/vulkan/VulkanCommandPool.h>

namespace flux
{
    VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex)
        : logicalDevice_(device)
    {
        DBG_ASSERT(logicalDevice_, "Logical device cannot be null");

        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.queueFamilyIndex = queueFamilyIndex;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkResult result = vkCreateCommandPool(logicalDevice_, &poolCreateInfo, nullptr, &commandPool_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create command pool");
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
    }

    VkCommandBuffer VulkanCommandPool::AllocateCommandBuffer() const
    {
        VkCommandBuffer cmdBuffer;

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool_;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(logicalDevice_, &allocateInfo, &cmdBuffer);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create command buffer");

        return cmdBuffer;
    }

    std::vector<VkCommandBuffer> VulkanCommandPool::AllocateCommandBuffer(uint32_t count) const
    {
        std::vector<VkCommandBuffer> commandBuffers(count);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool_;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = count;

        VkResult result = vkAllocateCommandBuffers(logicalDevice_, &allocateInfo, commandBuffers.data());
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create command buffer");

        return commandBuffers;
    }

    void VulkanCommandPool::BeginCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer");
    }

    void VulkanCommandPool::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue) const
    {
        constexpr uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

        DBG_ASSERT(commandBuffer, "Command buffer cannot be null");

        VkResult result = vkEndCommandBuffer(commandBuffer);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to end command buffer");

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkFenceCreateInfo fCreateInfo{};
        fCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fCreateInfo.flags = 0;

        VkFence fence;
        result = vkCreateFence(logicalDevice_, &fCreateInfo, nullptr, &fence);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create fence");

        result = vkQueueSubmit(queue, 1, &submitInfo, fence);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to submit queue");

        result = vkWaitForFences(logicalDevice_, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
        DBG_ASSERT(result == VK_SUCCESS, "");   // idk even what bad could happen with this func

        vkDestroyFence(logicalDevice_, fence, nullptr);
        vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &commandBuffer);
    }

    void VulkanCommandPool::FreeCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &commandBuffer);
    }

    void VulkanCommandPool::FreeCommandBuffer(const std::vector<VkCommandBuffer>& commandBuffers) const
    {
        vkFreeCommandBuffers(logicalDevice_, commandPool_, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }
}