#pragma once

#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanRenderer.h>

namespace flux
{
    // static initialization
    RendererAPI::Type RendererAPI::currentAPI = RendererAPI::Type::Vulkan;

    Ref<RendererAPI> RendererAPI::Create(const Ref<GraphicsContext>& graphicsContext)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanRenderer>(graphicsContext);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "Invalid renderer API");
        return nullptr;
    }
}