#include <fxpch.h>

#include <platform/vulkan/VulkanRenderCommandBuffer.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    VulkanRenderCommandBuffer::VulkanRenderCommandBuffer(const Ref<GraphicsContext>& graphicsContext)
        : context_(graphicsContext)
    {
        commandPool_ = VulkanContext::Device()->CommandPool();
        commandBuffers_ = commandPool_->AllocateCommandBuffer(VulkanFramesInFlight());
    }

    VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
    {
        VulkanContext::Device()->Idle();
        commandPool_->FreeCommandBuffer(commandBuffers_);
    }

    void VulkanRenderCommandBuffer::Begin()
    {
        uint32_t index = std::dynamic_pointer_cast<VulkanContext>(context_)->Swapchain()->CurrentFrameIndex();
        commandPool_->BeginCommandBuffer(commandBuffers_[index]);
    }

    void VulkanRenderCommandBuffer::End()
    {
        uint32_t index = std::dynamic_pointer_cast<VulkanContext>(context_)->Swapchain()->CurrentFrameIndex();
        vkEndCommandBuffer(commandBuffers_[index]);
    }

    void VulkanRenderCommandBuffer::Begin(uint32_t frameInFlight)
    {
        commandPool_->BeginCommandBuffer(commandBuffers_[frameInFlight]);
    }

    void VulkanRenderCommandBuffer::End(uint32_t frameInFlight)
    {
        vkEndCommandBuffer(commandBuffers_[frameInFlight]);
    }

    VkCommandBuffer VulkanRenderCommandBuffer::GetNativeCommandBuffer() const
    {
        uint32_t index = std::dynamic_pointer_cast<VulkanContext>(context_)->Swapchain()->CurrentFrameIndex();
        return commandBuffers_[index];
    }
}