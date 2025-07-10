#pragma once

#include <flux/renderer/ShaderInputSet.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanShaderInputSet : public ShaderInputSet
    {
    public:
        VulkanShaderInputSet(const Ref<Shader>& shader, uint32_t set = 0);
        ~VulkanShaderInputSet();

        virtual uint32_t SetIndex() const override { return set_; }

        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding) const override;
        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding, uint32_t frameInFlight) const override;

        virtual void PushSampler(const Ref<Image>& image, uint32_t binding) const override;
        virtual void PushSampler(const Ref<Image>& image, uint32_t binding, uint32_t frameInFlight) const override;

        VkDescriptorSet DescriptorSet(uint32_t frameInFlight) const { return descriptorSets_[frameInFlight]; }

    private:
        std::vector<VkDescriptorSet> descriptorSets_;
        uint32_t set_;
    };
}