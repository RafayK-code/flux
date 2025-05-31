#include <pch.h>

#include <flux/renderer/Shader.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanShader.h>

namespace flux
{
    Ref<Shader> Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanShader>(vertexPath, fragmentPath);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}