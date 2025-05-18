#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanCommandBuffer : public RefCounted
    {
    public:
        VulkanCommandBuffer(VkCommandBuffer commandBuffer);

        void Begin();
        void End();

    private:
        VkCommandBuffer commandBuffer_;
    };
}