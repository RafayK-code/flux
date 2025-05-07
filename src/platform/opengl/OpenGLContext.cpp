#include <pch.h>

#include <platform/opengl/OpenGLContext.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h> 

namespace flux
{
    OpenGLContext::OpenGLContext(GLFWwindow* window)
        : windowHandle_(window)
    {
        DBG_ASSERT(windowHandle_, "Window cannot be null");

        glfwMakeContextCurrent(windowHandle_);

        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        DBG_ASSERT(status, "Failed to initialize Glad");

        DBG_PRINT("OpenGL Info:");
        DBG_PRINT("Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        DBG_PRINT("Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        DBG_PRINT("Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(windowHandle_);
    }

    void OpenGLContext::PreWindowCreateHints()
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef FLUX_PLATFORM_APPLE
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }
}