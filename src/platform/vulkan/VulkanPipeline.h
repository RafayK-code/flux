#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <platform/vulkan/VulkanDevice.h>

namespace flux
{
    // this should be done a lot different - tbh this should be abstracted to GraphicsPipeline instead of vulkan only
    struct PipelineConfig
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;

        static void InitDefault(PipelineConfig& config, uint32_t width, uint32_t height);
    };

    class VulkanPipeline : public RefCounted
    {
    public:
        VulkanPipeline(const Ref<VulkanDevice>& device, const PipelineConfig& config);
        ~VulkanPipeline();

        void Bind(VkCommandBuffer commandBuffer);

    private:
        Ref<VulkanDevice> device_;

        VkPipeline pipeline_;

        // probably shouldnt be here either
        VkShaderModule vsModule_;
        VkShaderModule fsModule_;
    };
}