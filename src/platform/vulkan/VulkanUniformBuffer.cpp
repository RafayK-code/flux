#include <pch.h>

#include <platform/vulkan/VulkanUniformBuffer.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
        : size_(size)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size_;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        allocation_ = VulkanContext::Allocator().AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, vulkanBuffer_);

        descriptorInfo_.buffer = vulkanBuffer_;
        descriptorInfo_.offset = 0;
        descriptorInfo_.range = size_;
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        VulkanContext::Allocator().DestroyBuffer(vulkanBuffer_, allocation_);
    }

    void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        uint8_t* pData = VulkanContext::Allocator().MapMemory<uint8_t>(allocation_);
        memcpy(pData, (const uint8_t*)data + offset, size);
        VulkanContext::Allocator().UnmapMemory(allocation_);
    }
}