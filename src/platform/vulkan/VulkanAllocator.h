#pragma once

#include <platform/vulkan/VulkanDevice.h>

#include <vma/vk_mem_alloc.h>

#include <string>

namespace flux
{
    class VulkanAllocator
    {
    public:
        VulkanAllocator() = default;
        VulkanAllocator(const std::string& tag);
        ~VulkanAllocator() = default;

        static void Init(VkInstance instance, const Ref<VulkanDevice>& device);
        static void Shutdown();

        static VmaAllocator& GetVmaAllocator();

        VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer);
        VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize = nullptr);

        void Free(VmaAllocation allocation);
        void DestroyImage(VkImage image, VmaAllocation allocation);
        void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

        template <typename T>
        T* MapMemory(VmaAllocation allocation)
        {
            T* mappedMemory;
            vmaMapMemory(VulkanAllocator::GetVmaAllocator(), allocation, (void**)&mappedMemory);
            return mappedMemory;
        }

        void UnmapMemory(VmaAllocation allocation);

    private:
        std::string tag_;
    };
}