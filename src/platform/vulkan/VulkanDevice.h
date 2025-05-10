#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>
#include <string>
#include <set>

namespace flux
{
    class VulkanPhsyicalDevice : public RefCounted
    {
    public:
        VulkanPhsyicalDevice();
        ~VulkanPhsyicalDevice();

        bool IsExtensionSupported(const std::string& extensionName) const;
        uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

        VkPhysicalDevice GetNativePhysicalDevice() const { return physicalDevice_; }

        struct QueueFamilyIndices
        {
            uint32_t graphicsFamily;
            uint32_t presentFamily;
            bool graphicsFamilyHasValue = false;
            bool presentFamilyHasValue = false;

            bool Complete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
        };

        const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices_; }

    private:
        VkPhysicalDevice physicalDevice_;
        VkPhysicalDeviceProperties properties_;
        VkPhysicalDeviceFeatures features_;
        VkPhysicalDeviceMemoryProperties memoryProperties_;

        VkFormat depthFormat_;

        QueueFamilyIndices queueFamilyIndices_;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties_;
        std::set<std::string> supportedExtensions_;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos_;

        friend class VulkanDevice;
    };

    class VulkanCommandPool : public RefCounted
    {
    public:
        VulkanCommandPool();
        ~VulkanCommandPool();

        VkCommandBuffer AllocateCommandBuffer(bool begin);
        void FlushCommandBuffer(VkCommandBuffer commandBuffer);
        void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);

        VkCommandPool CommandPool() const { return commandPool_; }

    private:
        VkCommandPool commandPool_;
    };

    class VulkanDevice : public RefCounted
    {
    public:

    };
}