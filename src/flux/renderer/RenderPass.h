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
        static Ref<RenderPass> Create(const Ref<Pipeline>& pipeline);

        virtual ~RenderPass() = default;
    };
}