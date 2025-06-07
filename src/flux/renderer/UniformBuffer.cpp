#include <pch.h>

#include <flux/renderer/UniformBuffer.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanUniformBuffer.h>

namespace flux
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanUniformBuffer>(size);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}