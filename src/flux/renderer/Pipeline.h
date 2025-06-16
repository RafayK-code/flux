#pragma once

#include <flux/renderer/Shader.h>
#include <flux/renderer/Framebuffer.h>
#include <flux/renderer/RenderStates.h>
#include <flux/renderer/VertexBuffer.h>
#include <flux/renderer/GraphicsContext.h>

namespace flux
{
    enum class PrimitiveTopology
    {
        None = 0,
        Points,
        Lines,
        Triangles,
        LineStrip,
        TriangleStrip,
        TriangleFan
    };

    struct PipelineSpecification
    {
        Ref<Shader> shader;
        Ref<Framebuffer> framebuffer;   // if framebuffer is null, use swapchain
        Ref<GraphicsContext> graphicsContext;

        PrimitiveTopology topology = PrimitiveTopology::Triangles;
        DepthState depthState{};
        BlendState blendState{};
        VertexBufferLayout layout{};
    };

    class Pipeline : public RefCounted
    {
    public:
        static Ref<Pipeline> Create(const PipelineSpecification& specification);

        virtual ~Pipeline() = default;
        virtual void Invalidate() = 0;

        virtual PipelineSpecification& Specification() { return specification_; }
        virtual const PipelineSpecification& Specification() const { return specification_; }

        virtual Ref<Shader> GetShader() const { return specification_.shader; }

    protected:
        PipelineSpecification specification_;
    };
}