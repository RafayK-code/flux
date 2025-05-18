#pragma once

#include <flux/renderer/GraphicsContext.h>
#include <platform/vulkan/VulkanDevice.h>
#include <platform/vulkan/VulkanSwapchain.h>
#include <platform/vulkan/VulkanPipeline.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace flux
{
    /**
    * Some bad things happening here like the fact that this can be instantiated
    * multiple times... which means we could have more than 1 VkInstance and VkDevice
    * Ok for now, as long as we keep 1 window but keep this in mind if we ever have multiple windows
    */
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* window);
        virtual ~VulkanContext();

        virtual void BindContext() override {}
        virtual void SwapBuffers() override {}

        static void PreWindowCreateHints();

        static inline VkInstance Instance() { return vulkanInstance_; }
        static inline const Ref<VulkanDevice>& Device() { return device_; }
        static inline VulkanAllocator& Allocator() { return *allocator_; }
          
        inline VkSurfaceKHR Surface() const { return surface_; }
        inline const Ref<VulkanSwapchain>& Swapchain() const { return swapchain_; }

    private:
        void CreateInstance();
        void CreateDevice();

    private:
        static VkInstance vulkanInstance_;
        static Ref<VulkanDevice> device_;
        static Ref<VulkanPhysicalDevice> physicalDevice_;
        static VkDebugUtilsMessengerEXT debugUtilsMessenger_;
        static Scope<VulkanAllocator> allocator_;

        VkSurfaceKHR surface_;
        GLFWwindow* windowHandle_;
        Ref<VulkanSwapchain> swapchain_;

        // def does NOT belong here. Purely for testing
        //Ref<VulkanPipeline> pipeline_;
    };
}