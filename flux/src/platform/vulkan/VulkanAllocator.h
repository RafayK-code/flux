#pragma once

#include <platform/vulkan/VulkanDevice.h>

#include <vma/vk_mem_alloc.h>

#include <string>

namespace flux
{
    class VulkanAllocator
    {
    public:
        VulkanAllocator(VkInstance instance, const Ref<VulkanDevice>& device);
        ~VulkanAllocator();

        inline VmaAllocator GetVmaAllocator() { return allocator_; }

        VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer);
        VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize = nullptr);

        void Free(VmaAllocation allocation);
        void DestroyImage(VkImage image, VmaAllocation allocation);
        void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

        template <typename T>
        T* MapMemory(VmaAllocation allocation)
        {
            T* mappedMemory;
            vmaMapMemory(allocator_, allocation, (void**)&mappedMemory);
            return mappedMemory;
        }

        void UnmapMemory(VmaAllocation allocation);

    private:
        VmaAllocator allocator_;
    };
}