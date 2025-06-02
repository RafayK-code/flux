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

        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding) const override;
        virtual void PushSampler(const Ref<Image>& image, uint32_t binding) const override;

        VkShaderModule VertexShaderModule() const { return vsModule_; }
        VkShaderModule FragmentShaderModule() const { return fsModule_; }

        VkDescriptorSetLayout DescriptorSetLayout() const { return descriptorSetLayout_; }
        uint32_t DescriptorSetLayoutCount() const { return descriptorSetLayoutCount_; }

        VkDescriptorSet DescriptorSet() const { return descriptorSet_; }

    private:
        VkShaderModule vsModule_;
        VkShaderModule fsModule_;

        VkDescriptorSetLayout descriptorSetLayout_;
        uint32_t descriptorSetLayoutCount_;

        VkDescriptorSet descriptorSet_;
    };
}