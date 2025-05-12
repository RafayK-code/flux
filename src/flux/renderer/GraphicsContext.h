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

        // we need two phase init cuz of vulkan... arghhh
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static void PreWindowCreateHints();
    };
}