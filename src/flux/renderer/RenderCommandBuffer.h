#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/GraphicsContext.h>

namespace flux
{
    class RenderCommandBuffer : public RefCounted
    {
    public:
        static Ref<RenderCommandBuffer> Create(const Ref<GraphicsContext>& graphicsContext);

        virtual ~RenderCommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
}