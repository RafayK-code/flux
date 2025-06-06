#pragma once

#include <flux/renderer/Framebuffer.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanFramebuffer : public Framebuffer
    {
    public:
        VulkanFramebuffer(const FramebufferSpecification& specification);
        virtual ~VulkanFramebuffer();

        virtual void Invalidate() override;
        virtual void Release() override;

        virtual void Bind() const override {}
        virtual void Unbind() const override {}

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual uint32_t Width() const override { return width_; }
        virtual uint32_t Height() const override { return height_; }

        virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const override {}

        virtual Ref<Image> GetImage(uint32_t attachmentIndex = 0) const override { return attachmentImages_[attachmentIndex]; }
        virtual Ref<Image> GetDepthImage() const override { return depthAttachmentImage_; }

        virtual size_t GetColorAttachmentCount() const override { return attachmentImages_.size(); }
        virtual bool HasDepthAttachment() const override { return depthAttachmentImage_ != nullptr; }

        virtual const FramebufferSpecification& Specification() const override { return spec_; }

        VkFramebuffer NativeVulkanFramebuffer() const { return framebuffer_; }
        VkRenderPass NativeVulkanRenderPass() const { return renderPass_; }

    private:
        FramebufferSpecification spec_;
        uint32_t width_;
        uint32_t height_;

        std::vector<Ref<Image>> attachmentImages_;
        Ref<Image> depthAttachmentImage_;

        std::vector<VkClearValue> clearValues_;

        VkFramebuffer framebuffer_;
        VkRenderPass renderPass_;
    };
}