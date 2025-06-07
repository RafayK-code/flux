#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    enum class ImageFormat
    {
        RED8UN,
        RED8UI,
        RED16UI,
        RED32UI,
        RED32F,
        RG8,
        RG16F,
        RG32F,
        RGB,
        RGBA,
        RGBA16F,
        RGBA32F,

        B10R11G11UF,

        SRGB,
        SRGBA,

        DEPTH32FSTENCIL8UINT,
        DEPTH32F,
        DEPTH24STENCIL8,
    };

    enum class ImageUsage
    {
        Texture,
        Attachment,
        Storage,
    };

    enum class TextureWrapMode
    {
        Repeat,
        RepeatMirrored,
        Clamp,
        ClampEdge,
        ClampBorder,
    };

    enum class TextureFilterMode
    {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear,
    };

    struct ImageSpecification
    {
        ImageFormat format = ImageFormat::RGBA;
        ImageUsage usage = ImageUsage::Texture;
        bool transfer = false;

        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t mips = 1;
        uint32_t layers = 1;
        bool createSampler = true;
    };

    struct ImageSubresourceRange
    {
        uint32_t baseMip = 0;
        uint32_t mipCount = 1;
        uint32_t baseLayer = 0;
        uint32_t layerCount = 1;
    };

    class Image : public RefCounted
    {
    public:
        static Ref<Image> Create(const ImageSpecification& specification);
        static Ref<Image> Create(const ImageSpecification& specification, void* data, size_t size);

        virtual ~Image() = default;

        virtual void Invalidate() = 0;
        virtual void Release() = 0;

        virtual const ImageSpecification& Specification() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual uint32_t Width() const = 0;
        virtual uint32_t Height() const = 0;

        virtual bool HasMips() const = 0;

        virtual float AspectRatio() const = 0;

        virtual void SetData(void* data, size_t size) = 0;
    };

    bool FormatIsIntegerBased(const ImageFormat format);

    inline bool IsDepthFormat(ImageFormat format) { return format == ImageFormat::DEPTH24STENCIL8 || format == ImageFormat::DEPTH32F || format == ImageFormat::DEPTH32FSTENCIL8UINT; }
}