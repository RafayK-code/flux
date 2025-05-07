#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class FLUX_API GraphicsContext
    {
    public:
        static Scope<GraphicsContext> Create(void* window);

        virtual ~GraphicsContext() = default;

        virtual void SwapBuffers() = 0;

        static void PreWindowCreateHints();
    };
}