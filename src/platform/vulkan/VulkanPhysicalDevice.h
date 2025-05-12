#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <set>
#include <string>

#define FLUX_VK_USE_PRESENT_QUEUE

namespace flux
{
    /**
    * Wrapper class over a VkPhysicalDevice
    */
    class VulkanPhysicalDevice : public RefCounted
    {
    public:
        static Ref<VulkanPhysicalDevice> Select(VkInstance instance, VkSurfaceKHR surface);

        VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        ~VulkanPhysicalDevice();

        bool ExtensionSupported(const std::string& extensionName) const;
        uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

        VkPhysicalDevice NativePhysicalDevice() const { return physicalDevice_; }

#ifdef FLUX_VK_USE_PRESENT_QUEUE
        struct QueueFamilyIndices
        {
            uint32_t graphicsFamily = 0;
            uint32_t presentFamily = 0;
            bool graphicsFamilyHasValue = false;
            bool presentFamilyHasValue = false;

            bool Complete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
        };
#else
        struct QueueFamilyIndices
        {
            uint32_t graphicsFamily = 0;
            bool graphicsFamilyHasValue = false;

            bool Complete() const { return graphicsFamilyHasValue; }
        };
#endif

        const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices_; }

    private:
        VkPhysicalDevice physicalDevice_;
        VkPhysicalDeviceProperties properties_;
        VkPhysicalDeviceFeatures features_;
        VkPhysicalDeviceMemoryProperties memoryProperties_;

        //VkFormat depthFormat_;

        QueueFamilyIndices queueFamilyIndices_;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties_;
        std::set<std::string> supportedExtensions_;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos_;

        friend class VulkanDevice;
    };
}