#include <pch.h>

#include <flux/renderer/RenderCommandBuffer.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanRenderCommandBuffer.h>

namespace flux
{
    Ref<RenderCommandBuffer> RenderCommandBuffer::Create(const Ref<GraphicsContext>& graphicsContext)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanRenderCommandBuffer>(graphicsContext);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}