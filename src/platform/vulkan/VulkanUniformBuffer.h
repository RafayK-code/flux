#pragma once

#include <flux/renderer/UniformBuffer.h>

#include <platform/vulkan/VulkanAllocator.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanUniformBuffer : public UniformBuffer
    {
    public:
        VulkanUniformBuffer(uint32_t size);
        virtual ~VulkanUniformBuffer();

        virtual void Bind(uint32_t binding, uint32_t set = 0) const override {}
        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

        const VkDescriptorBufferInfo& DescriptorBufferInfo() const { return descriptorInfo_; }

    private:
        VkBuffer vulkanBuffer_;
        VmaAllocation allocation_;
        uint32_t size_;
        VkDescriptorBufferInfo descriptorInfo_{};
    };
}