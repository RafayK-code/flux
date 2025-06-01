#pragma once

#include <flux/renderer/IndexBuffer.h>

#include <platform/vulkan/VulkanAllocator.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        VulkanIndexBuffer(const uint32_t* data, uint32_t count);
        virtual ~VulkanIndexBuffer();

        virtual void Bind() const override {};
        virtual void Unbind() const override {};

        virtual uint32_t Count() const override { return count_; }

    private:
        VkBuffer vulkanBuffer_;
        VmaAllocation allocation_;

        uint32_t count_;
    };
}