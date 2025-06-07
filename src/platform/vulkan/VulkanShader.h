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

        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding, uint32_t index = 0) const override;
        virtual void PushSampler(const Ref<Image>& image, uint32_t binding, uint32_t index = 0) const override;

        VkShaderModule VertexShaderModule() const { return vsModule_; }
        VkShaderModule FragmentShaderModule() const { return fsModule_; }

        VkDescriptorSetLayout DescriptorSetLayout() const { return descriptorSetLayout_; }
        uint32_t DescriptorSetLayoutCount() const { return 1; }

        VkDescriptorSet DescriptorSet(uint32_t index) const { return descriptorSets_[index]; }

    private:
        VkShaderModule vsModule_;
        VkShaderModule fsModule_;

        VkDescriptorSetLayout descriptorSetLayout_;
        std::vector<VkDescriptorSet> descriptorSets_;
    };
}