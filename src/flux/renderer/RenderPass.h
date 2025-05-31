#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class RenderPass : public RefCounted
    {
    public:
        static Ref<RenderPass> Create();

        virtual ~RenderPass() = default;
    };
}