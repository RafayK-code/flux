#include <pch.h>

#include <platform/vulkan/VulkanFramebuffer.h>
#include <platform/vulkan/VulkanImage.h>

namespace flux
{
    static VkAttachmentLoadOp FluxAttachmentLoadOpToVulkan(const FramebufferSpecification& spec, const FramebufferTextureSpecification& attachmentSpec);

    VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specification)
        : spec_(specification), framebuffer_(nullptr), renderPass_(nullptr)
    {
        width_ = spec_.width * spec_.scale;
        height_ = spec_.height * spec_.scale;

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
            }
            else
            {
                attachmentImages_.emplace_back(Image::Create(spec));
            }
        }

        Invalidate();
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        Release();
    }

    void VulkanFramebuffer::Invalidate()
    {
        Release();

        bool createImages = attachmentImages_.empty();

        std::vector<VkAttachmentDescription> attachmentDescriptions;

        std::vector<VkAttachmentReference> colorAttachmentReferences;
        VkAttachmentReference depthAttachmentReference{};

        clearValues_.resize(spec_.attachments.attachments.size());

        uint32_t attachmentIndex = 0;
        for (const auto& attachmentSpec : spec_.attachments.attachments)
        {
            if (IsDepthFormat(attachmentSpec.format))
            {
                Ref<VulkanImage> depthAttachmentImage = std::dynamic_pointer_cast<VulkanImage>(depthAttachmentImage_);
                depthAttachmentImage->Resize(width_, height_);

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
                Ref<VulkanImage> colorAttachment;
                if (createImages)
                {
                    ImageSpecification spec;
                    spec.format = attachmentSpec.format;
                    spec.usage = ImageUsage::Attachment;
                    spec.transfer = false;  // needs to be better
                    spec.width = width_;
                    spec.height = height_;
                    colorAttachment = std::dynamic_pointer_cast<VulkanImage>(attachmentImages_.emplace_back(Image::Create(spec)));
                }
                else
                {
                    // when is this true..?
                    colorAttachment = std::dynamic_pointer_cast<VulkanImage>(attachmentImages_[attachmentIndex]);
                    // why..?
                    colorAttachment->Resize(width_, height_);
                }

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

                clearValues_[attachmentIndex].color = { 0.0f, 0.0f, 0.0f, 0.0f };   //TODO: actualy get proper clear color from client
                colorAttachmentReferences.emplace_back(VkAttachmentReference{ attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
            }

            attachmentIndex++;
        }
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