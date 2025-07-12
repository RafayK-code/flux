#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/GraphicsContext.h>
#include <flux/events/Event.h>

#include <string>

struct GLFWwindow;

namespace flux
{
    struct FLUX_API WindowProps
    {
        std::string title;
        uint32_t width;
        uint32_t height;
    };

    /**
    * Wrapper over a GLFW window
    * 
    * Directly coupled with GLFW - its highly unlikely that we will use a different windowing API.
    * If we decide to, we can abstract then
    */
    class FLUX_API Window
    {
    public:
        Window(const WindowProps& props);
        ~Window();

        void Update();

        inline uint32_t Width() const { return data_.width; }
        inline uint32_t Height() const { return data_.height; }

        void Resize(uint32_t width, uint32_t height);

        void SetVSync(bool enabled);
        inline bool IsVSync() const { return data_.vSync; }

        enum class Mode
        {
            Windowed,
            Borderless,
            Fullscreen,
        };

        void SetMode(Mode mode);
        inline Mode GetMode() const { return data_.mode; }

        inline GLFWwindow* GetNativePtr() const { return window_; }
        inline const Ref<GraphicsContext> Context() const { return data_.context; }

        inline EventDispatcher& Dispatcher() { return data_.dispatcher; }
        inline const EventDispatcher& Dispatcher() const { return data_.dispatcher; }

    private:
        GLFWwindow* window_;

        // need to make a struct so that we can toss it to glfw
        struct WindowData
        {
            std::string title = "Window";
            uint32_t width = 1600;
            uint32_t height = 900;
            Mode mode = Mode::Windowed;
            bool vSync = true;
            EventDispatcher dispatcher;
            Ref<GraphicsContext> context;
        };

        WindowData data_;
    };
}