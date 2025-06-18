#include <fxpch.h>

#include <flux/renderer/GraphicsContext.h>
#include <platform/opengl/OpenGLContext.h>
#include <platform/vulkan/VulkanContext.h>

#include <flux/renderer/RendererAPI.h>

namespace flux
{
    Ref<GraphicsContext> GraphicsContext::Create(void* window)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::OpenGL: return CreateRef<OpenGLContext>((GLFWwindow*)window);
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanContext>((GLFWwindow*)window);
        }
        
        DBG_ASSERT(false, "Invalid renderer API");
        return nullptr;
    }

    void GraphicsContext::PreWindowCreateHints()
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::OpenGL: OpenGLContext::PreWindowCreateHints(); break;
        case RendererAPI::Type::Vulkan: VulkanContext::PreWindowCreateHints(); break;
        }
    }
}