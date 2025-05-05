#pragma once

#include <flux/window/Window.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace flux
{
    class FLUX_API WindowGlfw : public Window
    {
    public:
        WindowGlfw(const WindowProps& props);
        virtual ~WindowGlfw();

        virtual void Update() override;

        virtual uint32_t Width() const override { return data_.width; }
        virtual uint32_t Height() const override { return data_.height; }

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void SetVSync(bool enabled) override;
        virtual bool IsVSync() const override { return data_.vSync; }

        virtual void SetMode(Mode mode) override;
        virtual Mode GetMode() const override { return data_.mode; }

        virtual void* GetNativePtr() const override { return window_; }

    private:
        GLFWwindow* window_;

        // need to make a struct so that we can toss it to glfw
        struct WindowData
        {
            std::string title;
            uint32_t width;
            uint32_t height;
            Mode mode;
            bool vSync;
        };

        WindowData data_;
    };
}