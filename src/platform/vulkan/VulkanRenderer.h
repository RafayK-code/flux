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
        virtual ~VulkanRenderer();

        virtual void BeginFrame() override;
        virtual void Present(const Ref<Image>& finalImage) override;

        virtual void Draw(const Ref<RenderPass>& renderPass, const Ref<VertexArray>& vertexArray) override;

    private:
        std::vector<VkCommandBuffer> commandBuffers_;
        Ref<GraphicsContext> graphicsContext_;

        Ref<RenderPass> swapchainRenderPass_;
        Ref<VertexArray> swapchainVertexArray_;
        uint32_t currentFrameInFlight_;

        std::vector<std::function<void(VkCommandBuffer cmd)>> drawCommands_;
    };
}