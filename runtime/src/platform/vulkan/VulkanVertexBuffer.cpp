#include <fxpch.h>

#include <platform/vulkan/VulkanVertexBuffer.h>

#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, uint64_t size, Usage usage)
        : size_(size), usage_(usage), data_(nullptr)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size_;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        allocation_ = VulkanContext::Allocator().AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, vulkanBuffer_);

        if (usage_ == Usage::Static)
        {
            void* dst = VulkanContext::Allocator().MapMemory<void>(allocation_);
            memcpy(dst, vertices, size_);
            VulkanContext::Allocator().UnmapMemory(allocation_);
        }
        else
        {
            data_ = VulkanContext::Allocator().MapMemory<uint8_t>(allocation_);
            memcpy(data_, vertices, size_);
        }
    }

    VulkanVertexBuffer::VulkanVertexBuffer(uint64_t size, Usage usage)
        : size_(size), usage_(usage), data_(nullptr)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        allocation_ = VulkanContext::Allocator().AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, vulkanBuffer_);
        if (usage_ == Usage::Dynamic)
            data_ = VulkanContext::Allocator().MapMemory<uint8_t>(allocation_);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        if (usage_ == Usage::Dynamic)
            VulkanContext::Allocator().UnmapMemory(allocation_);

        VulkanContext::Allocator().DestroyBuffer(vulkanBuffer_, allocation_);
    }

    void VulkanVertexBuffer::SetData(const void* data, uint64_t size, uint64_t offset)
    {
        if (usage_ == Usage::Static)
        {
            uint8_t* dst = VulkanContext::Allocator().MapMemory<uint8_t>(allocation_);
            memcpy(dst, (const uint8_t*)data + offset, size);
            VulkanContext::Allocator().UnmapMemory(allocation_);
        }
        else
        {
            memcpy(data_, (const uint8_t*)data + offset, size);
        }
    }
}