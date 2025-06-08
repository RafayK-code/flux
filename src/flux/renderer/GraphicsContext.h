#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class FLUX_API GraphicsContext : RefCounted
    {
    public:
        static Ref<GraphicsContext> Create(void* window);

        virtual ~GraphicsContext() = default;
        virtual void BindContext() = 0;
        virtual void SwapBuffers() = 0;

        virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t FramesInFlight() const = 0;

        static void PreWindowCreateHints();
    };
}