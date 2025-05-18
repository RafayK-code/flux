#include <pch.h>

#include <platform/vulkan/VulkanPipeline.h>

namespace flux
{
    static std::vector<char> ReadFile(const std::string& path);
    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& path);

    VulkanPipeline::VulkanPipeline(const Ref<VulkanDevice>& device, const PipelineConfig& config)
        : device_(device)
    {
        vsModule_ = CreateShaderModule(device_->NativeVulkanDevice(), "data/shaders/basic.vert.spv");
        fsModule_ = CreateShaderModule(device_->NativeVulkanDevice(), "data/shaders/basic.frag.spv");

        VkPipelineShaderStageCreateInfo shaderStages[2]{};

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vsModule_;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fsModule_;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        VkPipelineVertexInputStateCreateInfo vsInputInfo{};
        vsInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vsInputInfo.vertexAttributeDescriptionCount = 0;
        vsInputInfo.vertexBindingDescriptionCount = 0;
        vsInputInfo.pVertexAttributeDescriptions = nullptr;
        vsInputInfo.pVertexBindingDescriptions = nullptr;

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &config.viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &config.scissor;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vsInputInfo;
        pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &config.rasterizationInfo;
        pipelineInfo.pMultisampleState = &config.multisampleInfo;
        pipelineInfo.pColorBlendState = &config.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
        pipelineInfo.pDynamicState = nullptr;

        pipelineInfo.layout = config.pipelineLayout;
        pipelineInfo.renderPass = config.renderPass;
        pipelineInfo.subpass = config.subpass;

        VkResult result = vkCreateGraphicsPipelines(device_->NativeVulkanDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline_);

        DBG_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline");
    }

    VulkanPipeline::~VulkanPipeline()
    {
        vkDestroyShaderModule(device_->NativeVulkanDevice(), vsModule_, nullptr);
        vkDestroyShaderModule(device_->NativeVulkanDevice(), fsModule_, nullptr);
        vkDestroyPipeline(device_->NativeVulkanDevice(), pipeline_, nullptr);
    }

    void VulkanPipeline::Bind(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
    }

    void PipelineConfig::InitDefault(PipelineConfig& config, uint32_t width, uint32_t height)
    {
        config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        config.viewport.x = 0.0f;
        config.viewport.y = 0.0f;
        config.viewport.width = static_cast<float>(width);
        config.viewport.height = static_cast<float>(height);
        config.viewport.minDepth = 0.0f;
        config.viewport.maxDepth = 1.0f;

        config.scissor.offset = { 0, 0 };
        config.scissor.extent = { width, height };

        config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.rasterizationInfo.depthClampEnable = VK_FALSE;
        config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        config.rasterizationInfo.lineWidth = 1.0f;
        config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config.rasterizationInfo.depthBiasEnable = VK_FALSE;
        config.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        config.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        config.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.multisampleInfo.sampleShadingEnable = VK_FALSE;
        config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.multisampleInfo.minSampleShading = 1.0f;           // Optional
        config.multisampleInfo.pSampleMask = nullptr;             // Optional
        config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        config.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        config.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        config.colorBlendAttachment.blendEnable = VK_FALSE;
        config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.colorBlendInfo.logicOpEnable = VK_FALSE;
        config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        config.colorBlendInfo.attachmentCount = 1;
        config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
        config.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        config.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        config.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        config.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.depthStencilInfo.depthTestEnable = VK_TRUE;
        config.depthStencilInfo.depthWriteEnable = VK_TRUE;
        config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        config.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        config.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        config.depthStencilInfo.stencilTestEnable = VK_FALSE;
        config.depthStencilInfo.front = {};  // Optional
        config.depthStencilInfo.back = {};   // Optional
    }

    static std::vector<char> ReadFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        DBG_ASSERT(file.is_open(), "Couldnt open file");

        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(filesize);

        file.seekg(0);
        file.read(buffer.data(), filesize);

        return buffer;
    }

    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& path)
    {
        std::vector<char> shaderCode = ReadFile(path);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create shader module");

        return shaderModule;
    }
}