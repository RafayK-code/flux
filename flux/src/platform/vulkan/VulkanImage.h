#pragma once

#include <flux/renderer/Image.h>

#include <platform/vulkan/VulkanAllocator.h>

#include <vulkan/vulkan.h>

namespace flux
{
    class VulkanImage : public Image
    {
    public:
        VulkanImage(const ImageSpecification& specification);
        VulkanImage(const ImageSpecification& specification, void* data, size_t size);

        virtual ~VulkanImage();

        virtual void Invalidate() override;
        virtual void Release() override;

        virtual const ImageSpecification& Specification() const override { return spec_; }

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual uint32_t Width() const override { return spec_.width; }
        virtual uint32_t Height() const override { return spec_.height; }

        virtual bool HasMips() const override { return spec_.mips > 1; }

        virtual float AspectRatio() const override { return static_cast<float>(spec_.width) / static_cast<float>(spec_.height); }

        virtual void SetData(void* data, size_t size) override;

        virtual void CreatePerLayerImageViews() override;

        VkImage NativeVulkanImage() const { return image_; }
        VkImageView NativeVulkanImageView() const { return imageView_; }
        VkSampler NativeVulkanSampler() const { return sampler_; }

        void UpdateDescriptor();

        const VkDescriptorImageInfo& DescriptorImageInfo() const { return descriptorImageInfo_; }

    private:
        ImageSpecification spec_;

        VkImage image_;
        VkImageView imageView_;
        VkSampler sampler_;
        VmaAllocation allocation_;

        std::vector<VkImageView> perLayerImageViews_;
        std::map<uint32_t, VkImageView> perMipImageViews_;

        VkDescriptorImageInfo descriptorImageInfo_;
    };

    VkFormat FluxImageFormatToVulkan(ImageFormat format);
}