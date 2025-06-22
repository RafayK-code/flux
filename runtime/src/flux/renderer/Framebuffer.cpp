#include <fxpch.h>

#include <flux/renderer/Framebuffer.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanFramebuffer.h>

namespace flux
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanFramebuffer>(specification);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}