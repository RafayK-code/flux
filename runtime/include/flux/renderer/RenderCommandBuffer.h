#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/GraphicsContext.h>

namespace flux
{
    class FLUX_API RenderCommandBuffer : public RefCounted
    {
    public:
        static Ref<RenderCommandBuffer> Create(const Ref<GraphicsContext>& graphicsContext);

        virtual ~RenderCommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Begin(uint32_t frameInFlight) = 0;
        virtual void End(uint32_t frameInFlight) = 0;
    };
}