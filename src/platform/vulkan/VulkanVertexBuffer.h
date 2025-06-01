#pragma once

#include <flux/renderer/VertexBuffer.h>

#include <platform/vulkan/VulkanAllocator.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(const void* vertices, uint64_t size, Usage usage = Usage::Static);
        VulkanVertexBuffer(uint64_t size, Usage usage = Usage::Dynamic);

        virtual ~VulkanVertexBuffer();

        virtual void Bind() const override {};
        virtual void Unbind() const override {};

        virtual void SetData(const void* data, uint64_t size, uint64_t offset = 0) override;
        virtual uint64_t Size() const override { return size_; }

        VkBuffer NativeVulkanBuffer() const { return vulkanBuffer_; }

    private:
        VkBuffer vulkanBuffer_;
        VmaAllocation allocation_;

        uint64_t size_;
    };
}