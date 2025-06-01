#include <pch.h>

#include <flux/renderer/UniformBuffer.h>
#include <flux/renderer/RendererAPI.h>

namespace flux
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return nullptr;
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}