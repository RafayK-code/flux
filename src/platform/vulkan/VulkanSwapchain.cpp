#include <pch.h>

#include <platform/vulkan/VulkanSwapchain.h>
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

constexpr uint32_t FRAMES_IN_FLIGHT = 3;

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

    static void LoadVkFunctionPointers(VkInstance instance, VkDevice device);
    static void FindQueueFamilyIndex(uint32_t& graphicsFamilyIndex, uint32_t& presentFamilyIndex, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static void FindImageFormatAndColorSpace(VkFormat& colorFormat, VkColorSpaceKHR& colorSpace, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static VkRenderPass CreateRenderPass(VkDevice device, VkFormat colorFormat, VkFormat depthFormat);

    VulkanSwapchain::VulkanSwapchain(VkInstance instance, const Ref<VulkanDevice>& device, VkSurfaceKHR surface, VulkanSwapchainCreateProps& props)
        : instance_(instance)
        , device_(device)
        , surface_(surface)
        , queueFamilyIndex_(std::numeric_limits<uint32_t>::max())
        , vsync_(props.vsync)
        , currentFrameIndex_(0)
        , currentImageIndex_(0)
    {
        LoadVkFunctionPointers(instance, device_->NativeVulkanDevice());

        VkPhysicalDevice physicalDevice = device_->PhysicalDevice()->NativePhysicalDevice();

        uint32_t graphicsFamilyIndex, presentFamilyIndex;
        FindQueueFamilyIndex(graphicsFamilyIndex, presentFamilyIndex, physicalDevice, surface_);

        DBG_ASSERT(graphicsFamilyIndex != std::numeric_limits<uint32_t>::max(), "Couldnt find a graphics queue");
        DBG_ASSERT(graphicsFamilyIndex == presentFamilyIndex, "Flux does not support different queue familes for present and graphics");

        queueFamilyIndex_ = graphicsFamilyIndex;

        FindImageFormatAndColorSpace(colorFormat_, colorSpace_, physicalDevice, surface_);;

        VkDevice vulkanDevice = device_->NativeVulkanDevice();

        VkSurfaceCapabilitiesKHR surfCaps{};
        VkResult result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface_, &surfCaps);
        DBG_ASSERT(result == VK_SUCCESS, "");

        uint32_t presentModeCount;
        result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &presentModeCount, nullptr);
        DBG_ASSERT(result == VK_SUCCESS && presentModeCount > 0, "");

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &presentModeCount, presentModes.data());
        DBG_ASSERT(result == VK_SUCCESS, "");

        VkExtent2D swapchainExtent{};
        if (surfCaps.currentExtent.width == std::numeric_limits<uint32_t>::max())
        {
            swapchainExtent.width = props.width;
            swapchainExtent.height = props.height;
        }
        else
        {
            swapchainExtent = surfCaps.currentExtent;
            props.width = swapchainExtent.width;
            props.height = swapchainExtent.height;
        }

        width_ = props.width;
        height_ = props.height;

        DBG_ASSERT(width_ > 0 && height_ > 0, "invalid parameters for width and height");

        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        // look for a better fit
        VkPresentModeKHR otherPresentMode = vsync_ ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
        for (uint32_t i = 0; i < presentModeCount; i++)
        {
            if (presentModes[i] == otherPresentMode)
            {
                swapchainPresentMode = otherPresentMode;
                break;
            }
        }

        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        if (surfCaps.maxImageCount > 0 && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
            desiredNumberOfSwapchainImages = surfCaps.maxImageCount;

        VkSurfaceTransformFlagsKHR preTransform = surfCaps.currentTransform;
        if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

        // surfaces arent always opaque interestingly enough...
        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };

        for (auto compositeAlphaFlag : compositeAlphaFlags)
        {
            if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag)
            {
                compositeAlpha = compositeAlphaFlag;
                break;
            };
        }

        // finally
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.surface = surface_;
        swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCreateInfo.imageFormat = colorFormat_;
        swapchainCreateInfo.imageColorSpace = colorSpace_;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;      // ignored due to sharing mode
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;  // ignored due to sharing mode
        swapchainCreateInfo.presentMode = swapchainPresentMode;
        swapchainCreateInfo.oldSwapchain = nullptr;         // BAD - need a way to recreate a swap chain from an old one
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.compositeAlpha = compositeAlpha;

        if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        result = fpCreateSwapchainKHR(vulkanDevice, &swapchainCreateInfo, nullptr, &swapchain_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create swapchain");

        result = fpGetSwapchainImagesKHR(vulkanDevice, swapchain_, &imageCount_, nullptr);
        DBG_ASSERT(result == VK_SUCCESS, "");

        vulkanImages_.resize(imageCount_);
        result = fpGetSwapchainImagesKHR(vulkanDevice, swapchain_, &imageCount_, vulkanImages_.data());
        DBG_ASSERT(result == VK_SUCCESS, "");

        images_.resize(imageCount_);
        for (uint32_t i = 0; i < imageCount_; i++)
        {
            VkImageViewCreateInfo colorAttachmentView{};
            colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorAttachmentView.pNext = nullptr;
            colorAttachmentView.format = colorFormat_;
            colorAttachmentView.image = vulkanImages_[i];
            colorAttachmentView.components = {
                VK_COMPONENT_SWIZZLE_R,
                VK_COMPONENT_SWIZZLE_G,
                VK_COMPONENT_SWIZZLE_B,
                VK_COMPONENT_SWIZZLE_A
            };
            colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorAttachmentView.subresourceRange.baseMipLevel = 0;
            colorAttachmentView.subresourceRange.levelCount = 1;
            colorAttachmentView.subresourceRange.baseArrayLayer = 0;
            colorAttachmentView.subresourceRange.layerCount = 1;
            colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorAttachmentView.flags = 0;

            images_[i].image = vulkanImages_[i];

            result = vkCreateImageView(vulkanDevice, &colorAttachmentView, nullptr, &images_[i].imageView);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create image view: {0}", i);
        }

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkFormat depthFormat = device_->PhysicalDevice()->DepthFormat();

        renderPass_ = CreateRenderPass(vulkanDevice, colorFormat_, depthFormat);

        depthImages_.resize(imageCount_);

        for (uint32_t i = 0; i < imageCount_; i++)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width_;
            imageInfo.extent.height = height_;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            depthImages_[i].memoryAlloc = VulkanContext::Allocator().AllocateImage(imageInfo, VMA_MEMORY_USAGE_AUTO, depthImages_[i].image);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImages_[i].image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            vkCreateImageView(vulkanDevice, &viewInfo, nullptr, &depthImages_[i].imageView);
        }
        
        VkImageView imageAttachments[2]{};

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass_;
        framebufferCreateInfo.attachmentCount = 2;
        framebufferCreateInfo.width = width_;
        framebufferCreateInfo.height = height_;
        framebufferCreateInfo.layers = 1;

        framebuffers_.resize(imageCount_);
        for (uint32_t i = 0; i < imageCount_; i++)
        {
            imageAttachments[0] = images_[i].imageView;
            imageAttachments[1] = depthImages_[i].imageView;

            framebufferCreateInfo.pAttachments = imageAttachments;
            result = vkCreateFramebuffer(vulkanDevice, &framebufferCreateInfo, nullptr, &framebuffers_[i]);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer: {0}", i);
        }

        imageAvailableSemaphores_.resize(FRAMES_IN_FLIGHT);
        renderFinishedSemaphores_.resize(FRAMES_IN_FLIGHT);
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            result = vkCreateSemaphore(vulkanDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores_[i]);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create image semaphore: {0}", i);

            result = vkCreateSemaphore(vulkanDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores_[i]);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create render semaphore: {0}", i);
        }

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        inFlightFences_.resize(FRAMES_IN_FLIGHT);
        imagesInFlight_.resize(imageCount_);

        for (auto& fence : inFlightFences_)
        {
            result = vkCreateFence(vulkanDevice, &fenceCreateInfo, nullptr, &fence);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create fence");
        }
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        device_->Idle();
        VkDevice device = device_->NativeVulkanDevice();

        for (const auto& framebuffer : framebuffers_)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

        for (const auto& depthImage : depthImages_)
        {
            vkDestroyImageView(device, depthImage.imageView, nullptr);
            VulkanContext::Allocator().DestroyImage(depthImage.image, depthImage.memoryAlloc);
        }

        for (const auto& image : images_)
            vkDestroyImageView(device, image.imageView, nullptr);

        if (renderPass_)
            vkDestroyRenderPass(device, renderPass_, nullptr);

        if (swapchain_)
            fpDestroySwapchainKHR(device, swapchain_, nullptr);

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores_[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores_[i], nullptr);
            vkDestroyFence(device, inFlightFences_[i], nullptr);
        }
    }

    uint32_t VulkanSwapchain::AcquireNextImage()
    {
        //currentFrameIndex_ = (currentFrameIndex_ + 1) % FRAMES_IN_FLIGHT;

        VkResult result = vkWaitForFences(device_->NativeVulkanDevice(), 1, &inFlightFences_[currentFrameIndex_], VK_TRUE, std::numeric_limits<uint64_t>::max());
        DBG_ASSERT(result == VK_SUCCESS, "");

        uint32_t imageIndex;
        result = fpAcquireNextImageKHR(device_->NativeVulkanDevice(), swapchain_, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores_[currentFrameIndex_], nullptr, &imageIndex);

        if (result != VK_SUCCESS)
        {
            DBG_ASSERT(false, "");  // supposed to recreate swapchain here and test again
        }

        currentImageIndex_ = imageIndex;
        return imageIndex;
    }

    void VulkanSwapchain::Present(const VkCommandBuffer* commandBuffers)
    {
        VkDevice device = device_->NativeVulkanDevice();

        if (imagesInFlight_[currentImageIndex_] != nullptr)
            vkWaitForFences(device, 1, &imagesInFlight_[currentImageIndex_], VK_TRUE, std::numeric_limits<uint64_t>::max());

        imagesInFlight_[currentImageIndex_] = inFlightFences_[currentFrameIndex_];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores_[currentFrameIndex_];
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[currentFrameIndex_];

        vkResetFences(device, 1, &inFlightFences_[currentFrameIndex_]);
        VkResult result = vkQueueSubmit(device_->GraphicQueue(), 1, &submitInfo, inFlightFences_[currentFrameIndex_]);
        DBG_ASSERT(result == VK_SUCCESS, "");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores_[currentFrameIndex_];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain_;
        presentInfo.pImageIndices = &currentImageIndex_;

        result = fpQueuePresentKHR(device_->GraphicQueue(), &presentInfo);
        DBG_ASSERT(result == VK_SUCCESS, "");

        currentFrameIndex_ = (currentFrameIndex_ + 1) % FRAMES_IN_FLIGHT;
    }

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

    static void FindQueueFamilyIndex(uint32_t& graphicsFamilyIndex, uint32_t& presentFamilyIndex, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps.data());

        graphicsFamilyIndex = std::numeric_limits<uint32_t>::max();
        presentFamilyIndex = std::numeric_limits<uint32_t>::max();

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
    }

    static void FindImageFormatAndColorSpace(VkFormat& colorFormat, VkColorSpaceKHR& colorSpace, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        uint32_t formatCount;
        VkResult result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        DBG_ASSERT(result == VK_SUCCESS && formatCount > 0, "Physical device format count retrieval failed");

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
        DBG_ASSERT(result == VK_SUCCESS, "Surface fomat retrieval count");

        if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
        {
            colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
            colorSpace = surfaceFormats[0].colorSpace;
        }
        else
        {
            bool found_B8G8R8A8_UNORM = false;
            for (const auto& surfaceFormat : surfaceFormats)
            {
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    colorFormat = surfaceFormat.format;
                    colorSpace = surfaceFormat.colorSpace;
                    found_B8G8R8A8_UNORM = true;
                    break;
                }
            }

            if (!found_B8G8R8A8_UNORM)
            {
                colorFormat = surfaceFormats[0].format;
                colorSpace = surfaceFormats[0].colorSpace;
            }
        }
    }

    static VkRenderPass CreateRenderPass(VkDevice device, VkFormat colorFormat, VkFormat depthFormat)
    {
        VkAttachmentDescription attachments[2]{};

        VkAttachmentDescription& colorAttachmentDesc = attachments[0];
        colorAttachmentDesc.format = colorFormat;
        colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorReference{};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription& depthAttachmentDesc = attachments[1];
        depthAttachmentDesc.format = depthFormat;
        depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthReference{};
        depthReference.attachment = 1;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc{};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &colorReference;
        subpassDesc.pDepthStencilAttachment = &depthReference;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDesc;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkRenderPass renderPass;
        VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create render pass");

        return renderPass;
    }
}