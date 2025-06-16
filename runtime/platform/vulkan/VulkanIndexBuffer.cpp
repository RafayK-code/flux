#include <pch.h>

#include <platform/vulkan/VulkanIndexBuffer.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t* data, uint32_t count)
        : count_(count)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(uint32_t) * count_;
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        allocation_ = VulkanContext::Allocator().AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, vulkanBuffer_);

        uint32_t* dst = VulkanContext::Allocator().MapMemory<uint32_t>(allocation_);
        memcpy(dst, data, sizeof(uint32_t) * count_);
        VulkanContext::Allocator().UnmapMemory(allocation_);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        VulkanContext::Allocator().DestroyBuffer(vulkanBuffer_, allocation_);
    }
}