#include <pch.h>

#include <flux/renderer/IndexBuffer.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanIndexBuffer.h>

namespace flux
{
    Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* data, uint32_t count)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanIndexBuffer>(data, count);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}