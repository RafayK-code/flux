#include <pch.h>

#include <platform/vulkan/VulkanShaderInputSet.h>
#include <platform/vulkan/VulkanShader.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanUniformBuffer.h>
#include <platform/vulkan/VulkanImage.h>

namespace flux
{
    VulkanShaderInputSet::VulkanShaderInputSet(const Ref<Shader>& shader, uint32_t set)
    {
        Ref<VulkanShader> vkShader = std::dynamic_pointer_cast<VulkanShader>(shader);
        VkDescriptorSetLayout shaderLayout = vkShader->DescriptorSetLayout(set);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(VulkanFramesInFlight());
        for (uint32_t i = 0; i < VulkanFramesInFlight(); i++)
            descriptorSetLayouts[i] = shaderLayout;

        Ref<VulkanDescriptorPool> descriptorPool = VulkanContext::Device()->DescriptorPool();
        descriptorSets_ = descriptorPool->AllocateDescriptorSet(descriptorSetLayouts);
    }

    VulkanShaderInputSet::~VulkanShaderInputSet()
    {
        Ref<VulkanDevice> device = VulkanContext::Device();
        device->Idle();

        device->DescriptorPool()->FreeDescriptorSet(descriptorSets_);
    }

    void VulkanShaderInputSet::PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding) const
    {
        Ref<VulkanUniformBuffer> vulkanUb = std::dynamic_pointer_cast<VulkanUniformBuffer>(ub);

        std::vector<VkWriteDescriptorSet> writes(descriptorSets_.size());
        for (size_t i = 0; i < writes.size(); i++)
        {
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].dstSet = descriptorSets_[i];
            writes[i].dstBinding = binding;
            writes[i].dstArrayElement = 0;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[i].descriptorCount = 1;
            writes[i].pBufferInfo = &vulkanUb->DescriptorBufferInfo();
        }

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    void VulkanShaderInputSet::PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding, uint32_t frameInFlight) const
    {
        Ref<VulkanUniformBuffer> vulkanUb = std::dynamic_pointer_cast<VulkanUniformBuffer>(ub);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSets_[frameInFlight];
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &vulkanUb->DescriptorBufferInfo();

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    void VulkanShaderInputSet::PushSampler(const Ref<Image>& image, uint32_t binding) const
    {
        Ref<VulkanImage> vulkanImage = std::dynamic_pointer_cast<VulkanImage>(image);

        std::vector<VkWriteDescriptorSet> writes(descriptorSets_.size());
        for (size_t i = 0; i < writes.size(); i++)
        {
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].dstSet = descriptorSets_[i];
            writes[i].dstBinding = binding;
            writes[i].dstArrayElement = 0;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[i].descriptorCount = 1;
            writes[i].pImageInfo = &vulkanImage->DescriptorImageInfo();
        }

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    void VulkanShaderInputSet::PushSampler(const Ref<Image>& image, uint32_t binding, uint32_t frameInFlight) const
    {
        Ref<VulkanImage> vulkanImage = std::dynamic_pointer_cast<VulkanImage>(image);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSets_[frameInFlight];
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &vulkanImage->DescriptorImageInfo();

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }
}