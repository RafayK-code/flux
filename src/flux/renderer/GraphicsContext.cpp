#include <pch.h>

#include <flux/renderer/GraphicsContext.h>
#include <platform/opengl/OpenGLContext.h>

namespace flux
{
    Scope<GraphicsContext> GraphicsContext::Create(void* window)
    {
        return CreateScope<OpenGLContext>((GLFWwindow*)window);
    }

    void GraphicsContext::PreWindowCreateHints()
    {
        OpenGLContext::PreWindowCreateHints();
    }
}