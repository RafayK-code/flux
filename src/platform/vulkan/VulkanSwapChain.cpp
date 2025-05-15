#include <pch.h>

#include <platform/vulkan/VulkanSwapChain.h>
#include <platform/vulkan/VulkanContext.h>

// Macro to get a procedure address based on a vulkan instance
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
    flux::fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); \
    DBG_ASSERT(flux::fp##entrypoint, "");                                     \
}

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
    flux::fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
    DBG_ASSERT(flux::fp##entrypoint, "");                                     \
}

namespace flux
{
    static PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    static PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    static PFN_vkQueuePresentKHR fpQueuePresentKHR;

    static void LoadVkFunctionPointers(VkInstance instance, VkDevice device)
    {
        GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
        GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
        GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
        GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
        GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);

        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    }

    VulkanSwapchain::VulkanSwapchain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapchainCreateProps& props)
        : instance_(instance), device_(device), surface_(surface), queueFamilyIndex_(std::numeric_limits<uint32_t>::max())
    {
        LoadVkFunctionPointers(instance, device_->NativeVulkanDevice());

        VkPhysicalDevice physicalDevice = device->PhysicalDevice()->NativePhysicalDevice();

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps.data());

        uint32_t graphicsFamilyIndex = std::numeric_limits<uint32_t>::max();
        uint32_t presentFamilyIndex = std::numeric_limits<uint32_t>::max();

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilyProps[i].queueCount > 0 && queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphicsFamilyIndex = i;

            VkBool32 presentSupport = true;
            fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

            if (queueFamilyProps[i].queueCount > 0 && presentSupport)
                presentFamilyIndex = i;

            if (graphicsFamilyIndex != std::numeric_limits<uint32_t>::max() && presentFamilyIndex != std::numeric_limits<uint32_t>::max())
                break;
        }

        DBG_ASSERT(graphicsFamilyIndex != std::numeric_limits<uint32_t>::max(), "Couldnt find a graphics queue");
        DBG_ASSERT(graphicsFamilyIndex == presentFamilyIndex, "Flux does not support different queue familes for present and graphics");

        queueFamilyIndex_ = graphicsFamilyIndex;

        FindImageFormatAndColorSpace();


    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        device_->Idle();

        VkDevice device = device_->NativeVulkanDevice();

        if (swapchain_)
            fpDestroySwapchainKHR(device, swapchain_, nullptr);
    }

    void VulkanSwapchain::FindImageFormatAndColorSpace()
    {
        VkPhysicalDevice physicalDevice = device_->PhysicalDevice()->NativePhysicalDevice();

        uint32_t formatCount;
        VkResult result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &formatCount, nullptr);

        DBG_ASSERT(result == VK_SUCCESS && formatCount > 0, "Physical device format count retrieval failed");

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &formatCount, surfaceFormats.data());
        DBG_ASSERT(result == VK_SUCCESS, "Surface fomat retrieval count");

        if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
        {
            colorFormat_ = VK_FORMAT_B8G8R8A8_UNORM;
            colorSpace_ = surfaceFormats[0].colorSpace;
        }
        else
        {
            bool found_B8G8R8A8_UNORM = false;
            for (const auto& surfaceFormat : surfaceFormats)
            {
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    colorFormat_ = surfaceFormat.format;
                    colorSpace_ = surfaceFormat.colorSpace;
                    found_B8G8R8A8_UNORM = true;
                    break;
                }
            }

            if (!found_B8G8R8A8_UNORM)
            {
                colorFormat_ = surfaceFormats[0].format;
                colorSpace_ = surfaceFormats[0].colorSpace;
            }
        }
    }
}