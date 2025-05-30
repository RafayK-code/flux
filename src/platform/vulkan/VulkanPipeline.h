#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/Pipeline.h>

#include <platform/vulkan/VulkanDevice.h>

namespace flux
{
    class VulkanPipeline : public Pipeline
    {
    public:
        VulkanPipeline(const PipelineSpecification& specification, VkRenderPass renderPass = nullptr, uint32_t subpass = 0);
        ~VulkanPipeline();

        void Bind(VkCommandBuffer commandBuffer) const;

    private:
        VkPipeline pipeline_;
        VkPipelineLayout pipelineLayout_;
    };
}