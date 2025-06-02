#include <pch.h>

#include <platform/vulkan/VulkanImage.h>
#include <platform/vulkan/VulkanContext.h>
#include <platform/vulkan/VulkanUtils.h>

namespace flux
{
    VulkanImage::VulkanImage(const ImageSpecification& specification)
        : spec_(specification), image_(nullptr), imageView_(nullptr), sampler_(nullptr), allocation_(nullptr), descriptorImageInfo_({})
    {
        Invalidate();
    }

    VulkanImage::VulkanImage(const ImageSpecification& specification, void* data, size_t size)
        : spec_(specification), image_(nullptr), imageView_(nullptr), sampler_(nullptr), allocation_(nullptr), descriptorImageInfo_({})
    {
        Invalidate();
        SetData(data, size);
    }

    VulkanImage::~VulkanImage()
    {
        Release();
    }

    void VulkanImage::Invalidate()
    {
        Release();
        
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        if (spec_.usage == ImageUsage::Attachment)
        {
            if (IsDepthFormat(spec_.format))
                usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        else if (spec_.usage == ImageUsage::Texture)
        {
            usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        else if (spec_.usage == ImageUsage::Storage)
        {
            usage |= VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        VkImageAspectFlags aspectMask = IsDepthFormat(spec_.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        if (spec_.format == ImageFormat::DEPTH24STENCIL8)
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

        VkFormat vulkanFormat = FluxImageFormatToVulkan(spec_.format);
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        // create the image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = vulkanFormat;
        imageInfo.extent.width = spec_.width;
        imageInfo.extent.height = spec_.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = spec_.mips;
        imageInfo.arrayLayers = spec_.layers;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;

        allocation_ = VulkanContext::Allocator().AllocateImage(imageInfo, memoryUsage, image_);

        // create the image view
        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = vulkanFormat;
        imageViewInfo.flags = 0;
        imageViewInfo.subresourceRange.aspectMask = aspectMask;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = spec_.mips;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = spec_.layers;
        imageViewInfo.image = image_;

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        VkResult result = vkCreateImageView(device, &imageViewInfo, nullptr, &imageView_);
        DBG_ASSERT(result == VK_SUCCESS, "Failed to create image view");

        /**
        * TODO: We need to create our sampler. For now its okay but when we do texture's we will need it
        */

        if (spec_.createSampler)
        {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.maxAnisotropy = 1.0f;

            if (FormatIsIntegerBased(spec_.format))
            {
                samplerInfo.magFilter = VK_FILTER_NEAREST;
                samplerInfo.minFilter = VK_FILTER_NEAREST;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            }
            else
            {
                samplerInfo.magFilter = VK_FILTER_LINEAR;
                samplerInfo.minFilter = VK_FILTER_LINEAR;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            }

            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 100.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            
            result = vkCreateSampler(device, &samplerInfo, nullptr, &sampler_);
            DBG_ASSERT(result == VK_SUCCESS, "Failed to create sampler");
        }

        UpdateDescriptor();
    }

    void VulkanImage::Release()
    {
        if (image_ == nullptr)
            return;

        VkDevice device = VulkanContext::Device()->NativeVulkanDevice();
        vkDestroySampler(device, sampler_, nullptr);
        vkDestroyImageView(device, imageView_, nullptr);

        VulkanContext::Allocator().DestroyImage(image_, allocation_);

        sampler_ = nullptr;
        imageView_ = nullptr;
        image_ = nullptr;
        allocation_ = nullptr;
    }

    void VulkanImage::Resize(uint32_t width, uint32_t height)
    {
        spec_.width = width;
        spec_.height = height;
        Invalidate();
    }

    void VulkanImage::SetData(void* data, size_t size)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation = VulkanContext::Allocator().AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer);

        uint8_t* dst = VulkanContext::Allocator().MapMemory<uint8_t>(stagingBufferAllocation);
        memcpy(dst, data, size);
        VulkanContext::Allocator().UnmapMemory(stagingBufferAllocation);

        Ref<VulkanDevice> device = VulkanContext::Device();
        Ref<VulkanCommandPool> commandPool = device->CommandPool();

        VkCommandBuffer copyCommand = commandPool->AllocateCommandBuffer();
        commandPool->BeginCommandBuffer(copyCommand);

        VkImageSubresourceRange subresourceRange = {};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;

        // Transition the texture image layout to transfer target, so we can safely copy our buffer data to it.
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image_;
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // Insert a memory dependency at the proper pipeline stages that will execute the image layout transition 
        // Source pipeline stage is host write/read exection (VK_PIPELINE_STAGE_HOST_BIT)
        // Destination pipeline stage is copy command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
        vkCmdPipelineBarrier(
            copyCommand,
            VK_PIPELINE_STAGE_HOST_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = spec_.width;
        bufferCopyRegion.imageExtent.height = spec_.height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = 0;

        // Copy mip levels from staging buffer
        vkCmdCopyBufferToImage(
            copyCommand,
            stagingBuffer,
            image_,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &bufferCopyRegion);

        InsertImageMemoryBarrier(copyCommand, image_,
            VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, descriptorImageInfo_.imageLayout,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            subresourceRange);

        device->FlushCommandBuffer(copyCommand);

        VulkanContext::Allocator().DestroyBuffer(stagingBuffer, stagingBufferAllocation);
        UpdateDescriptor();
    }

    void VulkanImage::UpdateDescriptor()
    {
        if (IsDepthFormat(spec_.format))
            descriptorImageInfo_.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        else if (spec_.usage == ImageUsage::Storage)
            descriptorImageInfo_.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        else
            descriptorImageInfo_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        if (spec_.usage == ImageUsage::Storage)
            descriptorImageInfo_.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        descriptorImageInfo_.imageView = imageView_;
        descriptorImageInfo_.sampler = sampler_;
    }

    VkFormat FluxImageFormatToVulkan(ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RED8UN:               return VK_FORMAT_R8_UNORM;
        case ImageFormat::RED8UI:               return VK_FORMAT_R8_UINT;
        case ImageFormat::RED16UI:              return VK_FORMAT_R16_UINT;
        case ImageFormat::RED32UI:              return VK_FORMAT_R32_UINT;
        case ImageFormat::RED32F:               return VK_FORMAT_R32_SFLOAT;
        case ImageFormat::RG8:                  return VK_FORMAT_R8G8_UNORM;
        case ImageFormat::RG16F:                return VK_FORMAT_R16G16_SFLOAT;
        case ImageFormat::RG32F:                return VK_FORMAT_R32G32_SFLOAT;
        case ImageFormat::RGBA:                 return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::SRGBA:                return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::RGBA16F:              return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::RGBA32F:              return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ImageFormat::B10R11G11UF:          return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case ImageFormat::DEPTH32FSTENCIL8UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case ImageFormat::DEPTH32F:             return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::DEPTH24STENCIL8:      return VulkanContext::Device()->PhysicalDevice()->DepthFormat();
        }

        DBG_ASSERT(false, "Unrecognized image format");
        return VK_FORMAT_UNDEFINED;
    }
}