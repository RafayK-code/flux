#pragma once

#include <flux/renderer/RenderCommandBuffer.h>

#include <platform/vulkan/VulkanCommandPool.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanRenderCommandBuffer : public RenderCommandBuffer
    {
    public:
        VulkanRenderCommandBuffer(const Ref<GraphicsContext>& graphicsContext);
        virtual ~VulkanRenderCommandBuffer();

        virtual void Begin() override;
        virtual void End() override;

        virtual void Begin(uint32_t frameInFlight) override;
        virtual void End(uint32_t frameInFlight) override;

        VkCommandBuffer GetNativeCommandBuffer() const;
        VkCommandBuffer GetNativeCommandBuffer(uint32_t frameIndex) const { return commandBuffers_[frameIndex]; }

    private:
        std::vector<VkCommandBuffer> commandBuffers_;
        Ref<VulkanCommandPool> commandPool_;
        Ref<GraphicsContext> context_;
    };
}