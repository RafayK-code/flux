#include <fxpch.h>

#include <platform/vulkan/VulkanPhysicalDevice.h>

namespace flux
{
    static VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

    Ref<VulkanPhysicalDevice> VulkanPhysicalDevice::Select(VkInstance instance, VkSurfaceKHR surface)
    {
        return CreateRef<VulkanPhysicalDevice>(instance, surface);
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
    {
        uint32_t gpuCount = 0;
        vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        DBG_ASSERT(gpuCount > 0, "");

        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());

        VkPhysicalDevice selectedPhysicalDevice = nullptr;
        for (VkPhysicalDevice device : physicalDevices)
        {
            vkGetPhysicalDeviceProperties(device, &properties_);
            if (properties_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selectedPhysicalDevice = device;
                break;
            }
        }

        if (!selectedPhysicalDevice)
        {
            DBG_WARN("Could not find discrete GPU");
            selectedPhysicalDevice = physicalDevices.back();
        }

        physicalDevice_ = selectedPhysicalDevice;
        DBG_PRINT("Physical device: {0}", properties_.deviceName);

        vkGetPhysicalDeviceFeatures(physicalDevice_, &features_);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties_);

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
        DBG_ASSERT(queueFamilyCount > 0, "");

        queueFamilyProperties_.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilyProperties_.data());

        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount, nullptr);
        if (extCount > 0)
        {
            std::vector<VkExtensionProperties> extensions(extCount);
            if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount, extensions.data()) == VK_SUCCESS)
            {
                DBG_PRINT("Selected physical device has {0} extensions", extCount);
                for (const auto& ext : extensions)
                {
                    supportedExtensions_.emplace(ext.extensionName);
                    DBG_PRINT("  {0}", ext.extensionName);
                }
            }
        }

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilyProperties_[i].queueCount > 0 && queueFamilyProperties_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                queueFamilyIndices_.graphicsFamily = i;

            if (queueFamilyIndices_.Complete())
                break;
        }

        // vulkan wants the mem addr so this cant be cosntexpr
        static const float defaultQueuePriority = 0.0f;
        std::set<uint32_t> uniqueQueueFamiles = { queueFamilyIndices_.graphicsFamily };

        for (uint32_t queueFamily : uniqueQueueFamiles)
        {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamily;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos_.push_back(queueInfo);
        }

        depthFormat_ = FindDepthFormat(physicalDevice_);
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
    }

    bool VulkanPhysicalDevice::ExtensionSupported(const std::string& extensionName) const
    {
        return supportedExtensions_.find(extensionName) != supportedExtensions_.end();
    }

    uint32_t VulkanPhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const
    {
        for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++)
        {
            if ((typeBits & 1) == 1)
            {
                if ((memoryProperties_.memoryTypes[i].propertyFlags & properties) == properties)
                    return i;
            }
            typeBits >>= 1;
        }

        DBG_ASSERT(false, "Could not find a suitable memory type!");
        return std::numeric_limits<uint32_t>::max();
    }

    static VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice)
    {
        // Since all depth formats may be optional, we need to find a suitable depth format to use
        // Start with the highest precision packed format
        std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
        };
        
        for (auto& format : depthFormats)
        {
            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
            // Format must support depth stencil attachment for optimal tiling
            if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                return format;
        }

        return VK_FORMAT_UNDEFINED;
    }
}