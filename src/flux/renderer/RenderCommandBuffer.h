#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    /**
    * no-op class for GL.
    * 
    * Not great because we perform a dyn dispatch to LITERALLY do nothing.
    * But this is such a core part of the vulkan API its unavoidable
    */
    class FLUX_API RenderCommandBuffer : public RefCounted
    {
    public:
        static Ref<RenderCommandBuffer> Create();

        virtual ~RenderCommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Submit() = 0;
    };
}