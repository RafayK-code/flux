#if 0
#include <pch.h>

#include <platform/vulkan/VulkanSwapChain.h>

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

    VulkanSwapChain::VulkanSwapChain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapChainCreateProps& props)
        : instance_(instance), device_(device), surface_(surface)
    {
        LoadVkFunctionPointers(instance, device_->NativeVulkanDevice());
    }
}
#endif