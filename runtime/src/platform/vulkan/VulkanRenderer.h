#pragma once

#include <flux/renderer/RendererAPI.h>
#include <flux/core/delegate/Delegate.h>

#include <platform/vulkan/VulkanSwapchain.h>

#include <vulkan/vulkan.h>
#include <functional>

namespace flux
{
    class VulkanRenderer : public RendererAPI
    {
    public:
        VulkanRenderer(const Ref<GraphicsContext>& graphicsContext);
        virtual ~VulkanRenderer() = default;

        virtual uint32_t BeginFrame() override;
        virtual void Present() override;

        virtual uint32_t CurrentFrameInFlight() const override { return currentFrameInFlight_; }

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void SubmitCommandBuffer(const Ref<RenderCommandBuffer>& commandBuffer) override;

        virtual void BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Framebuffer>& framebuffer) override;
        virtual void BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Framebuffer>& framebuffer) override;

        virtual void EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer) override;
        virtual void EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight) override;

        virtual void BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Pipeline>& pipeline) override;
        virtual void BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Pipeline>& pipeline) override;

        virtual void BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline) override;
        virtual void BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline) override;

        virtual void Draw(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        virtual void Draw(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

    private:
        Ref<GraphicsContext> context_;
        Ref<VulkanSwapchain> swapchain_;
        uint32_t currentFrameInFlight_;

        std::vector<VkCommandBuffer> commandsToExecute_;
        glm::ivec2 viewportOffset_;
        glm::uvec2 viewportExtent_;
    };
}