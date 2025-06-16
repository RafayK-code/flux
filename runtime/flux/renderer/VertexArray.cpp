#include <pch.h>

#include <flux/renderer/VertexArray.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanVertexArray.h>

namespace flux
{
    Ref<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanVertexArray>();
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }

    Ref<VertexArray> VertexArray::Create(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanVertexArray>(vb, ib);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}