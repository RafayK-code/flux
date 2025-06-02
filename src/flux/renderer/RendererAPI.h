#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class FLUX_API RendererAPI
    {
    public:
        enum class Type
        {
            None,
            OpenGL,
            Vulkan
        };

        //static Scope<RendererAPI> Create(Type type);
        static Type Current() { return currentAPI; }

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void DrawIndexed() = 0;

    private:
        static Type currentAPI;
    };
}