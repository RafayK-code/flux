#include <pch.h>

#include <platform/vulkan/VulkanDevice.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    VulkanDevice::VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
        : physicalDevice_(physicalDevice), enabledFeatures_(enabledFeatures), enableDebugMarkers_(false)
    {
        std::vector<const char*> deviceExtensions;

        DBG_ASSERT(physicalDevice_->ExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "");
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (physicalDevice_->ExtensionSupported(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME))
            deviceExtensions.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
        if (physicalDevice_->ExtensionSupported(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME))
            deviceExtensions.push_back(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(physicalDevice_->queueCreateInfos_.size());
        createInfo.pQueueCreateInfos = physicalDevice_->queueCreateInfos_.data();
        createInfo.pEnabledFeatures = &enabledFeatures_;

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
        
        if (physicalDevice_->ExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
        {
            deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
            enableDebugMarkers_ = true;
        }

        if (!deviceExtensions.empty())
        {
            createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        }

        VkResult result = vkCreateDevice(physicalDevice_->NativePhysicalDevice(), &createInfo, nullptr, &logicalDevice_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

        vkGetDeviceQueue(logicalDevice_, physicalDevice_->queueFamilyIndices_.graphicsFamily, 0, &graphicsQueue_);
        commandPool_ = CreateRef<VulkanCommandPool>(logicalDevice_, physicalDevice_->queueFamilyIndices_.graphicsFamily);
        descriptorPool_ = CreateRef<VulkanDescriptorPool>(logicalDevice_);
    }

    VulkanDevice::~VulkanDevice()
    {
        // command pool destructor needs to be called before device is destroyed
        DBG_ASSERT(commandPool_.use_count() <= 1, "Something owns the command pool when destroyed was called");
        vkDeviceWaitIdle(logicalDevice_);
        commandPool_.reset();
        descriptorPool_.reset();
        vkDestroyDevice(logicalDevice_, nullptr);
    }

    VkCommandBuffer VulkanDevice::AllocateCommandBuffer() const
    {
        return commandPool_->AllocateCommandBuffer();
    }

    void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        return commandPool_->FlushCommandBuffer(commandBuffer, graphicsQueue_);
    }

    void VulkanDevice::Idle() const
    {
        vkDeviceWaitIdle(logicalDevice_);
    }
}