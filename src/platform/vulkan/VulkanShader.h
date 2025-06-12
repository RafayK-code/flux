#pragma once

#include <flux/renderer/Shader.h>

#include <vulkan/vulkan.h>

#include <string>

namespace flux
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::string& vertexPath, const std::string& fragmentPath, const ShaderUniformLayout& layout);
        virtual ~VulkanShader();

        virtual void Bind() override {};
        virtual void Unbind() override {};

        VkShaderModule VertexShaderModule() const { return vsModule_; }
        VkShaderModule FragmentShaderModule() const { return fsModule_; }

        std::vector<VkDescriptorSetLayout> DescriptorSetLayout() const { return descriptorSetLayout_; }
        VkDescriptorSetLayout DescriptorSetLayout(uint32_t set) const { return descriptorSetLayout_[set]; }

    private:
        VkShaderModule vsModule_;
        VkShaderModule fsModule_;

        std::vector<VkDescriptorSetLayout> descriptorSetLayout_;
    };
}