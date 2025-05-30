#pragma once

#include <flux/renderer/Shader.h>

#include <vulkan/vulkan.h>

#include <string>

namespace flux
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::string& vertexPath, const std::string& fragmentPath);
        virtual ~VulkanShader();

        virtual void Bind() override {};
        virtual void Unbind() override {};

        VkShaderModule VertexShaderModule() const { return vsModule_; }
        VkShaderModule FragmentShaderModule() const { return fsModule_; }

    private:
        VkShaderModule vsModule_;
        VkShaderModule fsModule_;
    };
}