#include <pch.h>

#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanUtils.h>
#include <GLFW/glfw3.h>

namespace flux
{
#ifdef FLUX_DEBUG
    static bool enableValidationLayers = true;
#else
    static bool enableValidationLayers = false; // Let's leave this on for now...
#endif

    static constexpr const char* VkDebugUtilsMessageType(const VkDebugUtilsMessageTypeFlagsEXT type)
    {
        switch (type)
        {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:       return "General";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:    return "Validation";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:   return "Performance";
        default:                                                return "Unknown";
        }
    }

    static constexpr const char* VkDebugUtilsMessageSeverity(const VkDebugUtilsMessageSeverityFlagBitsEXT severity)
    {
        switch (severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:     return "error";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:   return "warning";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:      return "info";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:   return "verbose";
        default:                                                return "unknown";
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        (void)pUserData; //Unused argument

        const bool performanceWarnings = false;
        if (!performanceWarnings)
        {
            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
                return VK_FALSE;
        }

        std::string labels, objects;
        if (pCallbackData->cmdBufLabelCount)
        {
            labels = fmt::format("\tLabels({}): \n", pCallbackData->cmdBufLabelCount);
            for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i)
            {
                const auto& label = pCallbackData->pCmdBufLabels[i];
                const std::string colorStr = fmt::format("[ {}, {}, {}, {} ]", label.color[0], label.color[1], label.color[2], label.color[3]);
                labels.append(fmt::format("\t\t- Command Buffer Label[{0}]: name: {1}, color: {2}\n", i, label.pLabelName ? label.pLabelName : "NULL", colorStr));
            }
        }

        if (pCallbackData->objectCount)
        {
            objects = fmt::format("\tObjects({}): \n", pCallbackData->objectCount);
            for (uint32_t i = 0; i < pCallbackData->objectCount; ++i)
            {
                const auto& object = pCallbackData->pObjects[i];
                objects.append(fmt::format("\t\t- Object[{0}] name: {1}, type: {2}, handle: {3:#x}\n", i, object.pObjectName ? object.pObjectName : "NULL", VkObjectTypeToString(object.objectType), object.objectHandle));
            }
        }

        DBG_WARN("{0} {1} message: \n\t{2}\n {3} {4}", VkDebugUtilsMessageType(messageType), VkDebugUtilsMessageSeverity(messageSeverity), pCallbackData->pMessage, labels, objects);

        return VK_FALSE;
    }

    static bool CheckValidationLayerSupport(const char* validationLayer)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(validationLayer, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        return layerFound;
    }

    static std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    VulkanContext::VulkanContext()
        : debugUtilsMessenger_(nullptr)
    {
        VkApplicationInfo info{};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pApplicationName = ENGINE_NAME;
        info.pEngineName = ENGINE_NAME;
        info.apiVersion = VK_API_VERSION_1_2;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions2(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions2.data());

        std::vector<const char*> extensions = GetRequiredExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &info;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        const char* validationLayer = "VK_LAYER_KHRONOS_validation";
        if (enableValidationLayers)
        {
            if (CheckValidationLayerSupport(validationLayer))
            {
                createInfo.enabledLayerCount = 1;
                createInfo.ppEnabledLayerNames = &validationLayer;
            }
            else
            {
                DBG_ERROR("Validation layer [VK_LAYER_KHRONOS_validation] not present, validation is disabled");
            }
        }

        VkResult createRes = vkCreateInstance(&createInfo, nullptr, &vulkanInstance_);
        DBG_ASSERT(createRes == VK_SUCCESS, "Unable to create vulkan instance");

        if (enableValidationLayers)
        {
            PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance_, "vkCreateDebugUtilsMessengerEXT");
            DBG_ASSERT(vkCreateDebugUtilsMessengerEXT != NULL, "");
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
            debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugUtilsCreateInfo.pfnUserCallback = VulkanDebugUtilsMessengerCallback;
            debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

            createRes = vkCreateDebugUtilsMessengerEXT(vulkanInstance_, &debugUtilsCreateInfo, nullptr, &debugUtilsMessenger_);
            DBG_ASSERT(createRes == VK_SUCCESS, "Unable to create vulkan debug messenger");
        }
    }

    VulkanContext::~VulkanContext()
    {
        if (debugUtilsMessenger_)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(vulkanInstance_, "vkDestroyDebugUtilsMessengerEXT");

            vkDestroyDebugUtilsMessengerEXT(vulkanInstance_, debugUtilsMessenger_, nullptr);
            debugUtilsMessenger_ = nullptr;
        }

        vkDestroyInstance(vulkanInstance_, nullptr);
        vulkanInstance_ = nullptr;
    }

    void VulkanContext::PreWindowCreateHints()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
}