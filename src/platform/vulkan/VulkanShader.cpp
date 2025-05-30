#include <pch.h>

#include <platform/vulkan/VulkanShader.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& path);

    VulkanShader::VulkanShader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vsModule_ = CreateShaderModule(device, vertexPath);
        fsModule_ = CreateShaderModule(device, fragmentPath);
    }

    VulkanShader::~VulkanShader()
    {
        Ref<VulkanDevice> device = VulkanContext::Device();
        device->Idle();

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
}