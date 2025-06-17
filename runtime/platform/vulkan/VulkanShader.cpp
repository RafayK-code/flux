#include <fxpch.h>

#include <platform/vulkan/VulkanShader.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanUniformBuffer.h>
#include <platform/vulkan/VulkanImage.h>

namespace flux
{
    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& path);
    static VkDescriptorType FluxUniformTypeToVulkan(UniformType type);
    static VkShaderStageFlags FluxShaderStageFlagsToVulkan(ShaderStageFlags stage);

    VulkanShader::VulkanShader(const std::string& vertexPath, const std::string& fragmentPath, const ShaderUniformLayout& layout)
    {
        Ref<VulkanDevice> deviceRef = VulkanContext::Device();
        VkDevice device = deviceRef->NativeVulkanDevice();
        vsModule_ = CreateShaderModule(device, vertexPath);
        fsModule_ = CreateShaderModule(device, fragmentPath);

        descriptorSetLayout_.resize(layout.SetLayouts().size());
        for (size_t i = 0; i < descriptorSetLayout_.size(); i++)
        {
            const auto& setLayout = layout.SetLayouts()[i];
            std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

            for (const auto& element : setLayout)
            {
                VkDescriptorSetLayoutBinding binding{};
                binding.binding = element.binding;
                binding.descriptorType = FluxUniformTypeToVulkan(element.type);
                binding.stageFlags = FluxShaderStageFlagsToVulkan(element.shaderStage);
                binding.descriptorCount = 1;
                binding.pImmutableSamplers = nullptr;

                layoutBindings.push_back(binding);
            }

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
            layoutInfo.pBindings = layoutBindings.data();

            VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout_[i]);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout");
        }
    }

    VulkanShader::~VulkanShader()
    {
        Ref<VulkanDevice> device = VulkanContext::Device();
        device->Idle();

        for (const auto& layout : descriptorSetLayout_)
            vkDestroyDescriptorSetLayout(device->NativeVulkanDevice(), layout, nullptr);

        vkDestroyShaderModule(device->NativeVulkanDevice(), vsModule_, nullptr);
        vkDestroyShaderModule(device->NativeVulkanDevice(), fsModule_, nullptr);
    }

    static std::vector<uint8_t> ReadFileAsBytes(const std::string& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        DBG_ASSERT(file.is_open(), "Couldnt open file");

        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<uint8_t> buffer(filesize);

        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), filesize);

        return buffer;
    }

    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& path)
    {
        std::vector<uint8_t> shaderCode = ReadFileAsBytes(path);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create shader module");

        return shaderModule;
    }

    static VkDescriptorType FluxUniformTypeToVulkan(UniformType type)
    {
        switch (type)
        {
        case UniformType::UniformBuffer:    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case UniformType::Sampler:          return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        DBG_ASSERT(false, "Unknown uniform type");
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }

    static VkShaderStageFlags FluxShaderStageFlagsToVulkan(ShaderStageFlags stage)
    {
        VkShaderStageFlags flags = 0;

        if (stage & ShaderStage::Vertex)
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        if (stage & ShaderStage::Fragment)
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;

        return flags;
    }
}