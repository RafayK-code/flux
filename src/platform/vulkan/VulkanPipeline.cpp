#include <pch.h>

#include <platform/vulkan/VulkanPipeline.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanShader.h>

namespace flux
{
    static VkPrimitiveTopology FluxPrimitiveTopologyToVulkan(PrimitiveTopology topology);
    static VkBlendOp FluxBlendOpToVulkan(BlendOp blendOp);
    static VkBlendFactor FluxBlendFactorToVulkan(BlendFactor blendFactor);
    static VkCompareOp FluxDepthCompareOpToVulkan(DepthCompareOp depthOp);

    VulkanPipeline::VulkanPipeline(const PipelineSpecification& specification, VkRenderPass renderPass, uint32_t subpass)
    {
        specification_ = specification;
        Ref<VulkanDevice> device = VulkanContext::Device();

        //Ref<VulkanShader> vulkanShader = Ref<VulkanShader>(dynamic_cast<VulkanShader*>(specification_.shader.get()));
        Ref<VulkanShader> vulkanShader = std::dynamic_pointer_cast<VulkanShader>(specification_.shader);

        VkPipelineShaderStageCreateInfo shaderStages[2]{};

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vulkanShader->VertexShaderModule();
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = vulkanShader->FragmentShaderModule();
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

        // describe the vertex assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = FluxPrimitiveTopologyToVulkan(specification_.topology);
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // describe the rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        // describe multisampling (not really used for now, and shouldnt be used in a pixel game)
        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleInfo.minSampleShading = 1.0f;           // Optional
        multisampleInfo.pSampleMask = nullptr;             // Optional
        multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        // color blend attachment
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = specification_.blendState.enabled;// ? VK_TRUE : VK_FALSE; (implicit cast? not needed?)
        colorBlendAttachment.srcColorBlendFactor = FluxBlendFactorToVulkan(specification_.blendState.srcColorBlendFactor);
        colorBlendAttachment.dstColorBlendFactor = FluxBlendFactorToVulkan(specification_.blendState.dstColorBlendFactor);
        colorBlendAttachment.colorBlendOp = FluxBlendOpToVulkan(specification_.blendState.colorBlendOp);
        colorBlendAttachment.srcAlphaBlendFactor = FluxBlendFactorToVulkan(specification_.blendState.srcAlphaBlendFactor);
        colorBlendAttachment.dstAlphaBlendFactor = FluxBlendFactorToVulkan(specification_.blendState.dstAlphaBlendFactor);
        colorBlendAttachment.alphaBlendOp = FluxBlendOpToVulkan(specification_.blendState.alphaBlendOp);

        // actually creating the color blend info struct
        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachment;
        colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        // creating depth stencil info
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = specification_.depthState.enabled;
        depthStencilInfo.depthWriteEnable = VK_TRUE;
        depthStencilInfo.depthCompareOp = FluxDepthCompareOpToVulkan(specification_.depthState.depthCompareOp);
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        depthStencilInfo.stencilTestEnable = VK_FALSE;  // no stencil stuff :(
        depthStencilInfo.front = {};  // Optional
        depthStencilInfo.back = {};   // Optional

        // viewport create info (dynamic so we dont really create one)
        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = nullptr;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = nullptr;

        // dynamic viewport
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = 2;
        dynamicStateInfo.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vsInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;

        /**
        * TODO: Build the layout from the shader (UBO and push constant) and vertex array layout
        */
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        vkCreatePipelineLayout(device->NativeVulkanDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_);

        pipelineInfo.layout = pipelineLayout_;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = subpass;

        VkResult result = vkCreateGraphicsPipelines(device->NativeVulkanDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline_);

        DBG_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline");
    }

    VulkanPipeline::~VulkanPipeline()
    {
        Ref<VulkanDevice> device = VulkanContext::Device();
        device->Idle();

        vkDestroyPipeline(device->NativeVulkanDevice(), pipeline_, nullptr);
        vkDestroyPipelineLayout(device->NativeVulkanDevice(), pipelineLayout_, nullptr);
    }

    void VulkanPipeline::Bind(VkCommandBuffer commandBuffer) const
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
    }

    static VkPrimitiveTopology FluxPrimitiveTopologyToVulkan(PrimitiveTopology topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::Points:         return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveTopology::Lines:          return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::Triangles:      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveTopology::LineStrip:      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::TriangleStrip:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        }

        DBG_ASSERT(false, "Unknown primitive topology type");
        return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    }

    static VkBlendOp FluxBlendOpToVulkan(BlendOp blendOp)
    {
        switch (blendOp)
        {

        case BlendOp::Add:              return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:         return VK_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:  return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:              return VK_BLEND_OP_MIN;
        case BlendOp::Max:              return VK_BLEND_OP_MAX;
        }

        DBG_ASSERT(false, "Unknown blend op type");
        return VK_BLEND_OP_MAX_ENUM;
    }

    static VkBlendFactor FluxBlendFactorToVulkan(BlendFactor blendFactor)
    {
        switch (blendFactor)
        {
        case BlendFactor::Zero:                     return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:                      return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:                 return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:         return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:                 return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:         return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:                 return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:         return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:                 return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:         return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::OneMinusConstantColor:    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SrcAlphaSaturate:         return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        }

        DBG_ASSERT(false, "Unknown blend factor type");
        return VK_BLEND_FACTOR_MAX_ENUM;
    }

    static VkCompareOp FluxDepthCompareOpToVulkan(DepthCompareOp depthOp)
    {
        switch (depthOp)
        {
        case DepthCompareOp::Never:             return VK_COMPARE_OP_NEVER;
        case DepthCompareOp::NotEqual:          return VK_COMPARE_OP_NOT_EQUAL;
        case DepthCompareOp::Less:              return VK_COMPARE_OP_LESS;
        case DepthCompareOp::LessOrEqual:       return VK_COMPARE_OP_LESS_OR_EQUAL;
        case DepthCompareOp::Greater:           return VK_COMPARE_OP_GREATER;
        case DepthCompareOp::GreaterOrEqual:    return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case DepthCompareOp::Equal:             return VK_COMPARE_OP_EQUAL;
        case DepthCompareOp::Always:            return VK_COMPARE_OP_ALWAYS;
        }

        DBG_ASSERT(false, "Unknown depth compare op type");
        return VK_COMPARE_OP_MAX_ENUM;
    }
}