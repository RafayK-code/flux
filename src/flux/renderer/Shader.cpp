#include <pch.h>

#include <flux/renderer/Shader.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanShader.h>

namespace flux
{
    ShaderUniformElement::ShaderUniformElement(const std::string name, uint32_t binding, UniformType type, ShaderStageFlags shaderStage)
        : name(name), binding(binding), type(type), shaderStage(shaderStage)
    {
    }

    Ref<Shader> Shader::Create(const std::string& vertexPath, const std::string& fragmentPath, const ShaderUniformLayout& layout)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanShader>(vertexPath, fragmentPath, layout);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}