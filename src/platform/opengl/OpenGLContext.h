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

        virtual void SwapBuffers() override;

        static void PreWindowCreateHints();

    private:
        GLFWwindow* windowHandle_;
    };
}