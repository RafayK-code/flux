#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/RenderStates.h>
#include <flux/renderer/Image.h>

namespace flux
{
    enum class AttachmentLoadOp
    {
        Inherit, Clear, Load,
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(ImageFormat format) : format(format) {}

        ImageFormat format;
        BlendState blendMode{};
        AttachmentLoadOp loadOp = AttachmentLoadOp::Inherit;
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments) : attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> attachments;
    };

    struct FramebufferSpecification
    {
        float scale = 1.0f;
        uint32_t width = 0;
        uint32_t height = 0;
        
        float depthClearValue = 1.0f;
        bool clearColorOnLoad = true;
        bool clearDepthOnLoad = true;

        FramebufferAttachmentSpecification attachments;
        uint32_t samples = 1;
    };

    class Framebuffer : public RefCounted
    {
    public:
        static Ref<Framebuffer> Create(const FramebufferSpecification& specification);

        virtual ~Framebuffer() = default;

        virtual void Invalidate() = 0;
        virtual void Release() = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual uint32_t Width() const = 0;
        virtual uint32_t Height() const = 0;

        virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const = 0;

        virtual Ref<Image> GetColorImage(uint32_t attachmentIndex = 0) const = 0;
        virtual Ref<Image> GetDepthImage() const = 0;

        virtual size_t GetColorAttachmentCount() const = 0;
        virtual bool HasDepthAttachment() const = 0;

        virtual const FramebufferSpecification& Specification() const = 0;
    };
}