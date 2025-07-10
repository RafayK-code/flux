#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/GraphicsContext.h>
#include <flux/renderer/VertexArray.h>
#include <flux/renderer/RenderPass.h>
#include <flux/renderer/RenderCommandBuffer.h>
#include <flux/renderer/ShaderInputSet.h>

namespace flux
{
    class FLUX_API RendererAPI : public RefCounted
    {
    public:
        static Ref<RendererAPI> Create(const Ref<GraphicsContext>& graphicsContext);
        virtual ~RendererAPI() = default;

        enum class Type
        {
            None,
            OpenGL,
            Vulkan
        };

        /**
        * This is soooo problematic for so many reasons
        * Ideally we somehow make it so SetCurrent is only called once at start, and any future user changes
        * to change API instead stores that request persistently and is only refreshed on a game reload.
        * 
        * for now idc honestly
        */
        static void SetCurrent(Type type) { currentAPI = type; }

        static Type Current() { return currentAPI; }

        virtual uint32_t BeginFrame() = 0;
        virtual void Present() = 0;

        virtual uint32_t CurrentFrameInFlight() const = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SubmitCommandBuffer(const Ref<RenderCommandBuffer>& commandBuffer) = 0;

        virtual void BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Framebuffer>& framebuffer) = 0;
        virtual void BeginRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Framebuffer>& framebuffer) = 0;

        virtual void EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer) = 0;
        virtual void EndRenderPass(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight) = 0;

        virtual void BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<Pipeline>& pipeline) = 0;
        virtual void BindPipeline(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<Pipeline>& pipeline) = 0;

        virtual void BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline) = 0;
        virtual void BindShaderInputSet(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<ShaderInputSet>& inputSet, const Ref<Pipeline>& pipeline) = 0;

        virtual void Draw(const Ref<RenderCommandBuffer>& commandBuffer, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
        virtual void Draw(const Ref<RenderCommandBuffer>& commandBuffer, uint32_t frameInFlight, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

    private:
        static Type currentAPI;
    };
}