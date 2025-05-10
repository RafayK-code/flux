#include <pch.h>

#include <flux/renderer/GraphicsContext.h>
#include <platform/opengl/OpenGLContext.h>
#include <platform/vulkan/VulkanContext.h>

#include <flux/renderer/RendererAPI.h>

namespace flux
{
    Scope<GraphicsContext> GraphicsContext::Create(void* window)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::OpenGL: return CreateScope<OpenGLContext>((GLFWwindow*)window);
        case RendererAPI::Type::Vulkan: return CreateScope<VulkanContext>();
        }
        
        DBG_ASSERT(false, "Invalid renderer API");
        return nullptr;
    }

    void GraphicsContext::PreWindowCreateHints()
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::OpenGL: OpenGLContext::PreWindowCreateHints();
        case RendererAPI::Type::Vulkan: VulkanContext::PreWindowCreateHints();
        }
    }
}