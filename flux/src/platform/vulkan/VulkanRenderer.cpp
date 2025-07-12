#include <fxpch.h>

#include <platform/vulkan/VulkanRenderer.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanFramebuffer.h>
#include <platform/vulkan/VulkanVertexBuffer.h>
#include <platform/vulkan/VulkanIndexBuffer.h>
#include <platform/vulkan/VulkanShader.h>
#include <platform/vulkan/VulkanRenderCommandBuffer.h>
#include <platform/vulkan/VulkanShaderInputSet.h>

#include <glm/gtc/type_ptr.hpp>

namespace flux
{
    VulkanRenderer::VulkanRenderer(const Ref<GraphicsContext>& context)
        : context_(context), currentFrameInFlight_(0)
    {
        swapchain_ = std::dynamic_pointer_cast<VulkanContext>(context_)->Swapchain();
        viewportOffset_ = glm::ivec2(0, 0);
        viewportExtent_ = glm::uvec2(swapchain_->Width(), swapchain_->Height());
    }

    uint32_t VulkanRenderer::BeginFrame()
    {
        currentFrameInFlight_ = swapchain_->AcquireNextImage();
        return currentFrameInFlight_;
    }

    void VulkanRenderer::Present()
    {
        swapchain_->Present(commandsToExecute_);
        commandsToExecute_.clear();
    }

    void VulkanRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        viewportOffset_ = glm::ivec2(x, y);
        viewportExtent_ = glm::uvec2(width, height);
    }

    void VulkanRenderer::SubmitCommandBuffer(const Ref<RenderCommandBuffer>& commandBuffer)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);
        commandsToExecute_.push_back(vulkanCommandBuffer->GetNativeCommandBuffer(currentFrameInFlight_));
    }

    void VulkanRenderer::BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Framebuffer>& framebuffer)
    {
        BeginRenderPass(commandBuffer, currentFrameInFlight_, framebuffer);
    }

    void VulkanRenderer::BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Framebuffer>& framebuffer)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);

        Ref<VulkanFramebuffer> vkFramebuffer = std::dynamic_pointer_cast<VulkanFramebuffer>(framebuffer);
        Ref<VulkanSwapchain> swapchain = std::dynamic_pointer_cast<VulkanContext>(context_)->Swapchain();

        uint32_t width = vkFramebuffer ? vkFramebuffer->Width() : swapchain->Width();
        uint32_t height = vkFramebuffer ? vkFramebuffer->Height() : swapchain->Height();
        VkRenderPass nativeRenderPass = vkFramebuffer ? vkFramebuffer->NativeVulkanRenderPass() : swapchain->RenderPass();
        VkFramebuffer nativeFramebuffer = vkFramebuffer ? vkFramebuffer->NativeVulkanFramebuffer() : swapchain->GetFramebuffer(frameInFlight);

        float depthClearValue = vkFramebuffer ? vkFramebuffer->Specification().depthClearValue : 1.0f;

        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = nativeRenderPass;
        beginInfo.framebuffer = nativeFramebuffer;

        beginInfo.renderArea.offset = { 0, 0 };
        beginInfo.renderArea.extent = { width, height };

        VkClearValue clearValues[2]{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
        clearValues[1].depthStencil = { depthClearValue, 0 };

        beginInfo.clearValueCount = 2;
        beginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderer::EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer)
    {
        EndRenderPass(commandBuffer, currentFrameInFlight_);
    }

    void VulkanRenderer::EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);
        vkCmdEndRenderPass(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight));
    }

    void VulkanRenderer::BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Pipeline>& pipeline)
    {
        BindPipeline(commandBuffer, currentFrameInFlight_, pipeline);
    }

    void VulkanRenderer::BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Pipeline>& pipeline)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);
        Ref<VulkanPipeline> vkPipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
        vkCmdBindPipeline(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->NativePipeline());

        VkViewport viewport{};
        viewport.x = viewportOffset_.x;
        viewport.y = viewportOffset_.y;
        viewport.width = viewportExtent_.x;
        viewport.height = viewportExtent_.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { viewportOffset_.x, viewportOffset_.y };
        scissor.extent = { viewportExtent_.x, viewportExtent_.y };
        vkCmdSetScissor(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), 0, 1, &scissor);
    }

    void VulkanRenderer::BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline)
    {
        BindShaderInputSet(commandBuffer, currentFrameInFlight_, inputSet, pipeline);
    }

    void VulkanRenderer::BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);
        Ref<VulkanPipeline> vkPipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
        Ref<VulkanShaderInputSet> vkInputSet = std::dynamic_pointer_cast<VulkanShaderInputSet>(inputSet);

        VkDescriptorSet set = vkInputSet->DescriptorSet(frameInFlight);
        vkCmdBindDescriptorSets(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->PipelineLayout(), vkInputSet->SetIndex(), 1, &set, 0, nullptr);
    }

    void VulkanRenderer::Draw(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        Draw(commandBuffer, currentFrameInFlight_, vertexArray, indexCount);
    }

    void VulkanRenderer::Draw(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        Ref<VulkanRenderCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(commandBuffer);
        Ref<VulkanVertexBuffer> vertexBuffer = std::dynamic_pointer_cast<VulkanVertexBuffer>(vertexArray->GetVertexBuffer());
        VkDeviceSize offsets = 0;
        VkBuffer nativeVertexBuffer = vertexBuffer->NativeVulkanBuffer();
        vkCmdBindVertexBuffers(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), 0, 1, &nativeVertexBuffer, &offsets);

        Ref<VulkanIndexBuffer> indexBuffer = std::dynamic_pointer_cast<VulkanIndexBuffer>(vertexArray->GetIndexBuffer());
        vkCmdBindIndexBuffer(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), indexBuffer->NativeVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);

        uint32_t realIndexCount = indexCount > 0 ? indexCount : indexBuffer->Count();
        vkCmdDrawIndexed(vulkanCommandBuffer->GetNativeCommandBuffer(frameInFlight), realIndexCount, 1, 0, 0, 0);
    }
}