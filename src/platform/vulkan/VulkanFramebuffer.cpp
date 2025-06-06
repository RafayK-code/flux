#include <pch.h>

#include <platform/vulkan/VulkanFramebuffer.h>
#include <platform/vulkan/VulkanImage.h>
#include <platform/vulkan/VulkanContext.h>

namespace flux
{
    static VkAttachmentLoadOp FluxAttachmentLoadOpToVulkan(const FramebufferSpecification& spec, const FramebufferTextureSpecification& attachmentSpec);

    VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specification)
        : spec_(specification), framebuffer_(nullptr), renderPass_(nullptr)
    {
        width_ = spec_.width * spec_.scale;
        height_ = spec_.height * spec_.scale;

        Invalidate();
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        Release();
    }

    void VulkanFramebuffer::Invalidate()
    {
        Release();

        std::vector<VkAttachmentDescription> attachmentDescriptions;

        std::vector<VkAttachmentReference> colorAttachmentReferences;
        VkAttachmentReference depthAttachmentReference{};

        clearValues_.resize(spec_.attachments.attachments.size());

        uint32_t attachmentIndex = 0;
        for (const auto& attachmentSpec : spec_.attachments.attachments)
        {
            ImageSpecification spec;
            spec.format = attachmentSpec.format;
            spec.usage = ImageUsage::Attachment;
            spec.transfer = false;  // needs to be better
            spec.width = width_;
            spec.height = height_;

            if (IsDepthFormat(attachmentSpec.format))
            {
                depthAttachmentImage_ = Image::Create(spec);
                Ref<VulkanImage> depthAttachmentImage = std::dynamic_pointer_cast<VulkanImage>(depthAttachmentImage);

                VkAttachmentDescription attachmentDescription{};
                attachmentDescription.flags = 0;
                attachmentDescription.format = FluxImageFormatToVulkan(attachmentSpec.format);
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
                attachmentDescription.loadOp = FluxAttachmentLoadOpToVulkan(spec_, attachmentSpec);
                attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescription.initialLayout = attachmentDescription.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachmentDescriptions.push_back(attachmentDescription);

                depthAttachmentReference = { attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                clearValues_[attachmentIndex].depthStencil = { spec_.depthClearValue, 0 };
            }
            else
            {
                Ref<VulkanImage> colorAttachment = std::dynamic_pointer_cast<VulkanImage>(attachmentImages_.emplace_back(Image::Create(spec)));

                VkAttachmentDescription attachmentDescription{};
                attachmentDescription.flags = 0;
                attachmentDescription.format = FluxImageFormatToVulkan(attachmentSpec.format);
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
                attachmentDescription.loadOp = FluxAttachmentLoadOpToVulkan(spec_, attachmentSpec);
                attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescription.initialLayout = attachmentDescription.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachmentDescriptions.push_back(attachmentDescription);

                clearValues_[attachmentIndex].color = { 0.0f, 0.0f, 0.0f, 0.0f };   //TODO: actualy get proper clear color from client
                colorAttachmentReferences.emplace_back(VkAttachmentReference{ attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
            }

            attachmentIndex++;
        }

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
        subpassDescription.pColorAttachments = colorAttachmentReferences.data();
        if (depthAttachmentImage_)
            subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

        std::vector<VkSubpassDependency> dependencies;
        if (!attachmentImages_.empty())
        {
            {
                VkSubpassDependency& depedency = dependencies.emplace_back();
                depedency.srcSubpass = VK_SUBPASS_EXTERNAL;
                depedency.dstSubpass = 0;
                depedency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                depedency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
            {
                VkSubpassDependency& depedency = dependencies.emplace_back();
                depedency.srcSubpass = 0;
                depedency.dstSubpass = VK_SUBPASS_EXTERNAL;
                depedency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                depedency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        if (depthAttachmentImage_)
        {
            {
                VkSubpassDependency& depedency = dependencies.emplace_back();
                depedency.srcSubpass = VK_SUBPASS_EXTERNAL;
                depedency.dstSubpass = 0;
                depedency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                depedency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            {
                VkSubpassDependency& depedency = dependencies.emplace_back();
                depedency.srcSubpass = 0;
                depedency.dstSubpass = VK_SUBPASS_EXTERNAL;
                depedency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                depedency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create render pass");

        std::vector<VkImageView> attachments(attachmentImages_.size());
        for (uint32_t i = 0; i < attachmentImages_.size(); i++)
        {
            Ref<VulkanImage> image = std::dynamic_pointer_cast<VulkanImage>(attachmentImages_[i]);
            attachments[i] = image->NativeVulkanImageView();
        }

        if (depthAttachmentImage_)
        {
            Ref<VulkanImage> image = std::dynamic_pointer_cast<VulkanImage>(depthAttachmentImage_);
            attachments.push_back(image->NativeVulkanImageView());
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width_;
        framebufferInfo.height = height_;
        framebufferInfo.layers = 1;

        result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer_);
    }

    void VulkanFramebuffer::Release()
    {
        if (!framebuffer_)
            return;

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkDestroyFramebuffer(device, framebuffer_, nullptr);
        vkDestroyRenderPass(device, renderPass_, nullptr);

        attachmentImages_.clear();
        depthAttachmentImage_.reset();

        framebuffer_ = nullptr;
        renderPass_ = nullptr;
    }

    void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        spec_.width = width;
        spec_.height = height;

        width_ = width * spec_.scale;
        height_ = height * spec_.scale;

        Invalidate();
    }

    static VkAttachmentLoadOp FluxAttachmentLoadOpToVulkan(const FramebufferSpecification& spec, const FramebufferTextureSpecification& attachmentSpec)
    {
        if (attachmentSpec.loadOp == AttachmentLoadOp::Inherit)
        {
            if (IsDepthFormat(attachmentSpec.format))
                return spec.clearDepthOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;

            return spec.clearColorOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        return attachmentSpec.loadOp == AttachmentLoadOp::Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    }
}