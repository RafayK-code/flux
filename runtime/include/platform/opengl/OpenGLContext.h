#pragma once

#include <flux/renderer/GraphicsContext.h>

struct GLFWwindow;

namespace flux
{
    class FLUX_API OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* window);
        virtual ~OpenGLContext() = default;

        virtual void BindContext() override;
        virtual void SwapBuffers() override;

        virtual void OnWindowResize(uint32_t width, uint32_t height) override {}

        virtual uint32_t FramesInFlight() const override { return 1; }

        static void PreWindowCreateHints();

    private:
        GLFWwindow* windowHandle_;
    };
}