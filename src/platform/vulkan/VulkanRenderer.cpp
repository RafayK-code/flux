#include <pch.h>

#include <platform/vulkan/VulkanRenderer.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanFramebuffer.h>
#include <platform/vulkan/VulkanVertexBuffer.h>
#include <platform/vulkan/VulkanIndexBuffer.h>
#include <platform/vulkan/VulkanShader.h>

#include <glm/gtc/type_ptr.hpp>

namespace flux
{
    VulkanRenderer::VulkanRenderer(const Ref<GraphicsContext>& graphicsContext)
        : currentFrameInFlight_(0), graphicsContext_(graphicsContext)
    {
        Ref<VulkanCommandPool> commandPool = VulkanContext::Device()->CommandPool();
        commandBuffers_ = commandPool->AllocateCommandBuffer(VulkanFramesInFlight());

        float vertices[] = {
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,          // bottom left
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,           // bottom right
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,          // top right
            -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,         // top left
        };

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        uint32_t indexCount = sizeof(indices) / sizeof(uint32_t);

        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, indexCount);
        swapchainVertexArray_ = VertexArray::Create(vertexBuffer, indexBuffer);

        ShaderUniformLayout layout = {
            { "u_ViewProjection", 0, UniformType::UniformBuffer, ShaderStage::Vertex },
            { "u_Texture", 1, UniformType::Sampler, ShaderStage::Fragment },
        };
        Ref<Shader> shader = Shader::Create("assets/shaders/textured_quad.vert.spv", "assets/shaders/textured_quad.frag.spv", layout);

        glm::mat4 identity(1.0f);
        viewProjectionUB_ = UniformBuffer::Create(sizeof(float) * 16);
        viewProjectionUB_->SetData(glm::value_ptr(identity), sizeof(float) * 16);
        shader->PushUniformBuffer(viewProjectionUB_, 0);

        PipelineSpecification spec;
        spec.shader = shader;
        spec.layout = {
            { "a_Position", ShaderDataType::Float3 },
            { "a_Color", ShaderDataType::Float4 },
            { "a_TexCoord", ShaderDataType::Float2 },
        };

        // let the hacky bullshit begin!
        Ref<VulkanSwapchain> swapchain = std::dynamic_pointer_cast<VulkanContext>(graphicsContext_)->Swapchain();
        //Ref<Pipeline> pipeline = CreateRef<VulkanPipeline>(spec, swapchain->RenderPass());

        //swapchainRenderPass_ = CreateRef<RenderPass>(pipeline);
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Ref<VulkanDevice> device = VulkanContext::Device();
        device->Idle();
        device->CommandPool()->FreeCommandBuffer(commandBuffers_);
    }

    uint32_t VulkanRenderer::BeginFrame()
    {
        Ref<VulkanContext> vkContext = std::dynamic_pointer_cast<VulkanContext>(graphicsContext_);
        currentFrameInFlight_ = vkContext->Swapchain()->AcquireNextImage();
        return currentFrameInFlight_;
    }

    void VulkanRenderer::Present()
    {
        //swapchainRenderPass_->GetShader()->PushSampler(finalImage, 1, currentFrameInFlight_);
        //Draw(swapchainRenderPass_, swapchainVertexArray_);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkCommandBuffer cmd = commandBuffers_[currentFrameInFlight_];
        VkResult result = vkBeginCommandBuffer(cmd, &beginInfo);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer");

        for (auto& drawCommand : drawCommands_)
        {
            drawCommand(cmd);
        }

        result = vkEndCommandBuffer(cmd);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to end command buffer");

        Ref<VulkanSwapchain> swapchain = std::dynamic_pointer_cast<VulkanContext>(graphicsContext_)->Swapchain();
        swapchain->Present(&cmd);

        drawCommands_.clear();
    }

    void VulkanRenderer::Draw(const Ref<RenderPass>& renderPass, const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        drawCommands_.emplace_back([=](VkCommandBuffer cmd) {
            Ref<VulkanFramebuffer> framebuffer = std::dynamic_pointer_cast<VulkanFramebuffer>(renderPass->GetTargetFramebuffer());
            Ref<VulkanSwapchain> swapchain = std::dynamic_pointer_cast<VulkanContext>(graphicsContext_)->Swapchain();
            //auto framebuffer = framebufferWeak.lock();
            //auto swapchain = swapchainWeak.lock();

            uint32_t width = framebuffer ? framebuffer->Width() : swapchain->Width();
            uint32_t height = framebuffer ? framebuffer->Height() : swapchain->Height();
            VkRenderPass nativeRenderPass = framebuffer ? framebuffer->NativeVulkanRenderPass() : swapchain->RenderPass();
            VkFramebuffer nativeFramebuffer = framebuffer ? framebuffer->NativeVulkanFramebuffer() : swapchain->CurrentFramebuffer();

            float depthClearValue = framebuffer ? framebuffer->Specification().depthClearValue : 1.0f;

            VkRenderPassBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            beginInfo.renderPass = nativeRenderPass;
            beginInfo.framebuffer = nativeFramebuffer;

            beginInfo.renderArea.offset = { 0, 0 };
            beginInfo.renderArea.extent = { width, height };

            VkClearValue clearValues[2]{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
            clearValues[1].depthStencil = { depthClearValue, 0};

            beginInfo.clearValueCount = 2;
            beginInfo.pClearValues = clearValues;

            vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
            Ref<VulkanPipeline> pipeline = std::dynamic_pointer_cast<VulkanPipeline>(renderPass->GetPipeline());
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->NativePipeline());

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = width;
            viewport.height = height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = { width, height };
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            Ref<VulkanVertexBuffer> vertexBuffer = std::dynamic_pointer_cast<VulkanVertexBuffer>(vertexArray->GetVertexBuffer());
            VkDeviceSize offsets = 0;
            VkBuffer nativeVertexBuffer = vertexBuffer->NativeVulkanBuffer();
            vkCmdBindVertexBuffers(cmd, 0, 1, &nativeVertexBuffer, &offsets);

            Ref<VulkanIndexBuffer> indexBuffer = std::dynamic_pointer_cast<VulkanIndexBuffer>(vertexArray->GetIndexBuffer());
            vkCmdBindIndexBuffer(cmd, indexBuffer->NativeVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);

            Ref<VulkanShader> vkShader = std::dynamic_pointer_cast<VulkanShader>(renderPass->GetShader());
            VkDescriptorSet set = vkShader->DescriptorSet(currentFrameInFlight_);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->PipelineLayout(), 0, 1, &set, 0, nullptr);

            uint32_t realIndexCount = indexCount > 0 ? indexCount : indexBuffer->Count();
            vkCmdDrawIndexed(cmd, realIndexCount, 1, 0, 0, 0);

            vkCmdEndRenderPass(cmd);
        });
    }
}