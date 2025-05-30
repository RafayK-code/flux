#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>

namespace flux
{
    /**
    * For now, this is a non-owning reference to a command buffer
    * 
    * This may change
    */
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(VkCommandBuffer commandBuffer);
        ~VulkanCommandBuffer();

        void Begin();
        void End();

    private:
        VkCommandBuffer commandBuffer_;
    };
}