#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <string>

namespace flux
{
    struct WindowProps
    {
        std::string title;
        uint32_t width;
        uint32_t height;
    };

    /**
    * Interface with a static factory method that creates the correct concrete
    * implementation depending on the platform
    */
    class FLUX_API Window
    {
    public:
        static Scope<Window> Create(const WindowProps& props);

        virtual ~Window() = default;

        virtual void Update() = 0;

        virtual uint32_t Width() const = 0;
        virtual uint32_t Height() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        enum class Mode
        {
            Windowed,
            Borderless,
            Fullscreen,
        };

        virtual void SetMode(Mode mode) = 0;
        virtual Mode GetMode() const = 0;

        virtual void* GetNativePtr() const = 0;
    };
}