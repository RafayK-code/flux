#include <fxpch.h>

#include <platform/vulkan/VulkanAllocator.h>


namespace flux
{
    enum class AllocationType : uint8_t
    {
        None = 0,
        Buffer = 1,
        Image = 2,
    };

    struct AllocInfo
    {
        uint64_t allocatedSize = 0;
        AllocationType type = AllocationType::None;
    };

    static std::map<VmaAllocation, AllocInfo> allocationMap;

    VulkanAllocator::VulkanAllocator(VkInstance instance, const Ref<VulkanDevice>& device)
    {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorInfo.physicalDevice = device->PhysicalDevice()->NativePhysicalDevice();
        allocatorInfo.device = device->NativeVulkanDevice();
        allocatorInfo.instance = instance;

        vmaCreateAllocator(&allocatorInfo, &allocator_);
    }

    VulkanAllocator::~VulkanAllocator()
    {
        vmaDestroyAllocator(allocator_);
    }

    VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer)
    {
        DBG_ASSERT(bufferCreateInfo.size > 0, "Invalid buffer size");

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = usage;

        VmaAllocation allocation;
        vmaCreateBuffer(allocator_, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation, nullptr);
        if (allocation == nullptr)
        {
            DBG_ERROR("Failed to allocate GPU buffer: requested size = {0}", 0);    // fill in with bytes
        }

        VmaAllocationInfo allocInfo{};
        vmaGetAllocationInfo(allocator_, allocation, &allocInfo);
        DBG_WARN("VulkanAllocator: allocating buffer; size = {0}", 0);   // fill in with bytes

        return allocation;
    }

    VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize)
    {
        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = usage;

        VmaAllocation allocation;
        vmaCreateImage(allocator_, &imageCreateInfo, &allocCreateInfo, &outImage, &allocation, nullptr);
        if (allocation == nullptr)
        {
            DBG_ERROR("Failed to allocate GPU image: requested size = {0}x{1}x{2}", imageCreateInfo.extent.width, imageCreateInfo.extent.height, imageCreateInfo.extent.depth);
        }

        VmaAllocationInfo allocInfo{};
        vmaGetAllocationInfo(allocator_, allocation, &allocInfo);
        DBG_WARN("VulkanAllocator: allocating buffer; size = {0}", 0);   // fill in with bytes

        return allocation;
    }

    void VulkanAllocator::Free(VmaAllocation allocation)
    {
        vmaFreeMemory(allocator_, allocation);
    }

    void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
    {
        vmaDestroyBuffer(allocator_, buffer, allocation);
    }

    void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
    {
        vmaDestroyImage(allocator_, image, allocation);
    }

    void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
    {
        vmaUnmapMemory(allocator_, allocation);
    }
}