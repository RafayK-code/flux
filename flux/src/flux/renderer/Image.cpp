#include <fxpch.h>

#include <flux/renderer/Image.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanImage.h>

namespace flux
{
    Ref<Image> Image::Create(const ImageSpecification& specification)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanImage>(specification);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }

    Ref<Image> Image::Create(const ImageSpecification& specification, void* data, size_t size)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanImage>(specification, data, size);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }

    bool FormatIsIntegerBased(const ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RED16UI:
        case ImageFormat::RED32UI:
        case ImageFormat::RED8UI:
        case ImageFormat::DEPTH32FSTENCIL8UINT:
            return true;
        case ImageFormat::DEPTH32F:
        case ImageFormat::RED8UN:
        case ImageFormat::RGBA32F:
        case ImageFormat::B10R11G11UF:
        case ImageFormat::RG16F:
        case ImageFormat::RG32F:
        case ImageFormat::RED32F:
        case ImageFormat::RG8:
        case ImageFormat::RGBA:
        case ImageFormat::RGBA16F:
        case ImageFormat::RGB:
        case ImageFormat::SRGB:
        case ImageFormat::SRGBA:
        case ImageFormat::DEPTH24STENCIL8:
            return false;
        }
        
        DBG_ASSERT(false, "");
        return false;
    }
}