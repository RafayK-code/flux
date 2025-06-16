#include <pch.h>

#include <flux/renderer/ShaderInputSet.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanShaderInputSet.h>

namespace flux
{
    Ref<ShaderInputSet> ShaderInputSet::Create(const Ref<Shader>& shader, uint32_t set)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanShaderInputSet>(shader, set);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}