#pragma once

#include <flux/renderer/GraphicsContext.h>
#include <platform/vulkan/VulkanDevice.h>

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

        virtual void Init() override;
        virtual void SwapBuffers() override {}

        static void PreWindowCreateHints();

        inline VkInstance Instance() const { return vulkanInstance_; }
        inline VkSurfaceKHR Surface() const { return surface_; }

    private:
        VkInstance vulkanInstance_;
        VkDebugUtilsMessengerEXT debugUtilsMessenger_;
        VkSurfaceKHR surface_;

        Ref<VulkanDevice> device_;
        Ref<VulkanPhysicalDevice> physicalDevice_;

        GLFWwindow* windowHandle_;
    };
}