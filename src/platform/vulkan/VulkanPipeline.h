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
        // this renderpass override is a disgusting hack that should be addressed properly at some point
        VulkanPipeline(const PipelineSpecification& specification);
        virtual ~VulkanPipeline();

        void Bind(VkCommandBuffer commandBuffer) const;

        VkPipeline NativePipeline() const { return pipeline_; }
        VkPipelineLayout PipelineLayout() const { return pipelineLayout_; }

    private:
        VkPipeline pipeline_;
        VkPipelineLayout pipelineLayout_;
    };
}