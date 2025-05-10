#pragma once

#include <flux/renderer/GraphicsContext.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext();
        virtual ~VulkanContext();

        static inline VkInstance GetInstance() { return vulkanInstance_; }

        virtual void SwapBuffers() override {}

        static void PreWindowCreateHints();

    private:
        static inline VkInstance vulkanInstance_;

        VkDebugUtilsMessengerEXT debugUtilsMessenger_;
    };
}