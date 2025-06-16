#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/Pipeline.h>

namespace flux
{
    struct RenderPassSpecification
    {
        Ref<Pipeline> pipeline;
    };

    class RenderPass : public RefCounted
    {
    public:
        RenderPass(const Ref<Pipeline> pipeline) : pipeline_(pipeline) {}
        virtual ~RenderPass() = default;

        virtual Ref<Pipeline> GetPipeline() const { return pipeline_; }
        virtual Ref<Framebuffer> GetTargetFramebuffer() const { return pipeline_->Specification().framebuffer; }
        virtual Ref<Shader> GetShader() const { return pipeline_->GetShader(); }

    private:
        Ref<Pipeline> pipeline_;
    };
}